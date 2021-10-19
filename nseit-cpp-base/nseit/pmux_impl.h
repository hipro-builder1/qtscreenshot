/* Copyright (C) 2016-2018 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__8cc90d22_ce34_11e9_95c0_d43b04f4acea
#define NSEIT_CPP_BASE__8cc90d22_ce34_11e9_95c0_d43b04f4acea

#include <cstdint>

#include <boost/system/error_code.hpp>
#include <boost/asio/buffer.hpp>

#include "spdlog/spdlog.h"

#include "pmux.h"
#include "tcp-connection.h"
#include "periodic-updater.h"
#include "data-transport.h"
#include "msg-transport.h"
#include "clock.h"
#include "_namespace.h"

NAMESPACE__NSEIT__START

class PMuxImpl : public PMux,
                 public PeriodicUpdateReceiver,
                 public UpstreamDataTransport
{
public:
  PMuxImpl(
    std::shared_ptr<spdlog::logger> logger, Clock* clock, uint8_t version,
    const std::chrono::steady_clock::duration& heartbeat_interval,
    const std::chrono::steady_clock::duration& heartbeat_timeout);
  ~PMuxImpl() {}

  /*--- DI ---*/
  void set_upstream_transport(std::shared_ptr<UpstreamMsgTransport> transport)
  {
    m_upstream_transport = transport;
  }

  void set_connection(TCPConnection* connection)
  {
    m_connection = connection;
  }

  void set_periodic_updater(PeriodicUpdater* periodic_updater)
  {
    m_periodic_updater = periodic_updater;
  }

  /*--- PMux interface ---*/
  void start();

  /*--- DownstreamMsgTransport ---*/
  void send_message(uint8_t type, Buffer&& data);
  void drop_connection_from_upstream();
  void drop_connection();

  /*--- UpstreamDataTransport interface ---*/
  void got_data(const uint8_t* data, size_t length);
  void got_error(const boost::system::error_code& error);

  /*--- PeriodicUpdateReceiver interface ---*/
  void periodic_update(const std::chrono::steady_clock::time_point& now);

private:
#pragma pack(push, 1)
  struct pmux_header_s
  {
    uint32_t length;
    uint8_t type;
    uint8_t flags;
  };
#pragma pack(pop)

  static constexpr size_t HEADER_SIZE = sizeof(struct pmux_header_s);

  enum class State
  {
    READING_HEADER = 1,
    READING_PAYLOAD = 2,
  };

private:
  bool read_header(const uint8_t* data, size_t len, size_t* consumed);
  bool read_payload(const uint8_t* data, size_t len, size_t* consumed);
  bool handle_heartbeat(const struct pmux_header_s* header);
  void compute_next_heartbeat_interval(void);
  void send_heartbeat();

private:
  std::shared_ptr<spdlog::logger> m_logger;
  Clock* m_clock = nullptr;
  const uint8_t m_version;
  const std::chrono::steady_clock::duration m_heartbeat_interval;
  const std::chrono::steady_clock::duration m_heartbeat_timeout;

  std::shared_ptr<UpstreamMsgTransport> m_upstream_transport;
  TCPConnection* m_connection = nullptr;
  PeriodicUpdater* m_periodic_updater = nullptr;

  State m_state = State::READING_HEADER;
  struct pmux_header_s m_header;
  Buffer m_payload;
  size_t m_header_filled_bytes = 0;
  size_t m_payload_filled_bytes = 0;

  std::chrono::steady_clock::time_point m_last_incoming_data_ts;
  std::chrono::steady_clock::time_point m_last_outgoing_data_ts;
  bool m_first_run = true;
  std::chrono::steady_clock::duration m_computed_heartbeat_interval;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__8cc90d22_ce34_11e9_95c0_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
