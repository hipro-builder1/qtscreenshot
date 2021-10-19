/* Copyright (C) 2016-2018 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#include <stdlib.h>

#include <stdexcept>

#include "pmux_impl.h"
#include "assert.h"
#include "error.h"

NAMESPACE__NSEIT__START

namespace
{
static constexpr size_t MAX_MESSAGE_SIZE = (128 * 1024 * 1024); // 16MB

template<typename T1, typename T2>
long
milliseconds(const std::chrono::time_point<T1, T2>& t)
{
  auto epoch = t.time_since_epoch();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
  return ms.count();
}

template<typename T1, typename T2>
long
milliseconds(const std::chrono::duration<T1, T2>& d)
{
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(d);
  return ms.count();
}

template<typename T1, typename T2>
long
seconds(const std::chrono::duration<T1, T2>& d)
{
  auto ms = std::chrono::duration_cast<std::chrono::seconds>(d);
  return ms.count();
}

static constexpr uint8_t VERSION_MASK = 0x0F;
static constexpr uint8_t FLAG_HAS_PAYLOAD = 0x10;
static constexpr uint8_t FLAG_HEARTBEAT = 0x20;
} // namespace

PMuxImpl::PMuxImpl(
  std::shared_ptr<spdlog::logger> logger, Clock* clock, uint8_t version,
  const std::chrono::steady_clock::duration& heartbeat_interval,
  const std::chrono::steady_clock::duration& heartbeat_timeout)
  : m_logger(logger),
    m_clock(clock),
    m_version(version),
    m_heartbeat_interval(heartbeat_interval),
    m_heartbeat_timeout(heartbeat_timeout)
{
  if (m_version > 15) {
    throw std::logic_error("PMux version cannot exceed 4 bits");
  }
  m_upstream_transport = nullptr;
  m_connection = nullptr;
}

void
PMuxImpl::start()
{
  m_header_filled_bytes = 0;
  m_state = State::READING_HEADER;
  m_connection->start_reading();
  m_upstream_transport->connected();
}

void
PMuxImpl::send_message(uint8_t type, Buffer&& data)
{
  ASSERT((data.length() < MAX_MESSAGE_SIZE), "PMux payload too big");

  Buffer header_buffer(sizeof(pmux_header_s));
  pmux_header_s* header =
    reinterpret_cast<pmux_header_s*>(header_buffer.pointer());
  header->length = htonl(data.length());
  header->type = type;
  header->flags = m_version;

  m_connection->add_to_write_queue(std::move(header_buffer));
  if (data.length() != 0) {
    m_connection->add_to_write_queue(std::move(data));
  }
  m_connection->start_writing();
  m_last_outgoing_data_ts = m_clock->now();
}

void
PMuxImpl::drop_connection_from_upstream()
{
  m_periodic_updater->stop(this);
  m_connection->drop_connection_from_upstream();
}

void
PMuxImpl::drop_connection()
{
  auto error =
    error::make_error_code(error::Code::APPLICATION_INITIATED_DISCONNECT);
  m_upstream_transport->got_error(error);
  m_periodic_updater->stop(this);
  m_connection->drop_connection_from_upstream();
}

void
PMuxImpl::got_data(const uint8_t* data, size_t length)
{
  m_last_incoming_data_ts = m_clock->now();
  size_t consumed = 0;

  while (length) {
    bool bret = false;
    switch (m_state) {
    case State::READING_HEADER:
      bret = read_header(data, length, &consumed);
      break;

    case State::READING_PAYLOAD:
      bret = read_payload(data, length, &consumed);
      break;
    };

    if (bret == false) {
      // We encountered an error.
      auto error_code =
        error::make_error_code(error::Code::PMUX_PROTOCOL_ERROR);
      m_upstream_transport->got_error(error_code);
      m_periodic_updater->stop(this);
      m_connection->drop_connection_from_upstream();
      m_connection = nullptr;
      return;
    }

    ASSERT((consumed <= length), "Cannot consume more than what we have");
    length -= consumed;
    data += consumed;
    consumed = 0;
  }
}

void
PMuxImpl::got_error(const boost::system::error_code& error)
{
  /* We have nothing much to do here. We'll simply forward this
   * upstream. */
  m_upstream_transport->got_error(error);
  m_periodic_updater->stop(this);
}

bool
PMuxImpl::read_header(const uint8_t* data, size_t len, size_t* consumed)
{
  auto bytes_required = HEADER_SIZE - m_header_filled_bytes;
  auto bytes_to_read = bytes_required;
  if (bytes_required > len) {
    bytes_to_read = len;
  }

  auto to_buffer =
    reinterpret_cast<uint8_t*>(&m_header) + m_header_filled_bytes;
  memcpy(to_buffer, data, bytes_to_read);
  m_header_filled_bytes += bytes_to_read;
  *consumed = bytes_to_read;

  if (m_header_filled_bytes < HEADER_SIZE) {
    return true;
  }

  /* We got a header. Lets process it. */
  auto got_version = m_header.flags & VERSION_MASK;
  if (got_version != m_version) {
    m_logger->error(
      "Protocol version mismatch. Expected={}, got={}.", m_version,
      got_version);
    return false;
  }

  m_header.length = ntohl(m_header.length);
  if (m_header.length > MAX_MESSAGE_SIZE) {
    auto length = m_header.length;
    m_logger->error(
      "PMux payload too big. got-length={}, max-length={}.", length,
      MAX_MESSAGE_SIZE);
    return false;
  }

  if (m_header.flags & FLAG_HEARTBEAT) {
    return handle_heartbeat(&m_header);
  }

  if (m_header.type == 0) {
    m_logger->error("Invalid message-type '0'");
    return false;
  }

  if (m_header.length == 0) {
    m_upstream_transport->got_message(m_header.type, Buffer());
    m_header_filled_bytes = 0;
  } else {
    m_payload_filled_bytes = 0;
    m_payload = Buffer(m_header.length);
    m_state = State::READING_PAYLOAD;
  }

  return true;
}

bool
PMuxImpl::read_payload(const uint8_t* data, size_t len, size_t* consumed)
{
  auto bytes_required = m_header.length - m_payload_filled_bytes;
  auto bytes_to_read = bytes_required;
  if (bytes_required > len) {
    bytes_to_read = len;
  }

  auto payload_ptr = reinterpret_cast<uint8_t*>(m_payload.pointer());
  auto to_buffer = payload_ptr + m_payload_filled_bytes;
  memcpy(to_buffer, data, bytes_to_read);
  m_payload_filled_bytes += bytes_to_read;
  *consumed = bytes_to_read;

  if (m_payload_filled_bytes < m_header.length) {
    return true;
  }

  /* We got the payload */
  m_upstream_transport->got_message(m_header.type, std::move(m_payload));

  m_header_filled_bytes = 0;
  m_state = State::READING_HEADER;
  return true;
}

bool
PMuxImpl::handle_heartbeat(const struct pmux_header_s* header)
{
  if (header->length != 0) {
    m_logger->error("Heartbeat cannot have payload");
    return false;
  }
  m_header_filled_bytes = 0;
  m_state = State::READING_HEADER;
  return true;
}

void
PMuxImpl::send_heartbeat()
{
  Buffer header_buffer(HEADER_SIZE);
  pmux_header_s* header =
    reinterpret_cast<pmux_header_s*>(header_buffer.pointer());
  header->length = 0;
  header->type = 0;
  header->flags = (m_version | FLAG_HEARTBEAT);

  m_connection->add_to_write_queue(std::move(header_buffer));
  m_connection->start_writing();
}

void
PMuxImpl::periodic_update(const std::chrono::steady_clock::time_point& now)
{
  if (m_first_run == true) {
    m_first_run = false;
    m_last_incoming_data_ts = now;
    m_last_outgoing_data_ts = now;
    send_heartbeat();
    compute_next_heartbeat_interval();
    return;
  }

  /* If we havent gotten any data in the last heartbeat-timeout
   * seconds, we need to trigger a connection-drop. */
  auto tdiff = now - m_last_incoming_data_ts;
  if (tdiff > m_heartbeat_timeout) {
    m_logger->error("Heartbeat timeout reached. Dropping connection.");
    auto error =
      error::make_error_code(error::Code::HEARTBEAT_TIMEOUT_REACHED);
    m_upstream_transport->got_error(error);
    m_periodic_updater->stop(this);
    m_connection->drop_connection_from_upstream();
    return;
  }

  /* If there has been no data sent to the other side in the past
   * heartbeat-interval seconds, we need to send them something. */
  tdiff = now - m_last_outgoing_data_ts;
  if (tdiff >= m_computed_heartbeat_interval) {
    m_logger->trace(
      "Sending HB. now={}, last_sent_ts={}, tdiff={}, interval={}.",
      milliseconds(now), milliseconds(m_last_outgoing_data_ts),
      milliseconds(tdiff), milliseconds(m_computed_heartbeat_interval));

    send_heartbeat();
    m_last_outgoing_data_ts = now;
    compute_next_heartbeat_interval();
  }
}

void
PMuxImpl::compute_next_heartbeat_interval(void)
{
#if defined(WIN32) || defined(_WIN32) ||                                     \
  defined(__WIN32) && !defined(__CYGWIN__)
  auto random_val = rand();
#else
  auto random_val = random();
#endif
  auto r_median = (RAND_MAX / 2);
  auto r_relative = 0;
  if (random_val < r_median) {
    r_relative = random_val * -1;
  } else {
    r_relative = random_val - r_median;
  }
  auto r_percent = (1.0 * r_relative) / r_median;

  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
    m_heartbeat_interval);
  double tolerance = ms.count() * 0.50f;
  double backoff = tolerance * r_percent;
  auto new_interval = static_cast<long int>(ms.count() + backoff);
  m_computed_heartbeat_interval = std::chrono::milliseconds(new_interval);
}

NAMESPACE__NSEIT__END

/*
  Local Variables:
  mode: c++
  End:
*/
