/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#include <chrono>

#include <boost/asio/ip/address_v4.hpp>

#include "util.h"
#include "asio-tcp-connector.h"
#include "tcp-connection-manager.h"
#include "assert.h"

NAMESPACE__NSEIT__START

namespace asio = ::boost::asio;
namespace ip = ::boost::asio::ip;

AsioTCPConnector::AsioTCPConnector(
  std::shared_ptr<spdlog::logger> logger, asio::io_service& io_service,
  TCPConnectionFactory* connection_factory, const std::string& ip, int port)
  : m_logger(logger),
    m_io_service(io_service),
    m_connection_factory(connection_factory),
    m_ip(ip),
    m_port(port),
    m_endpoint(asio::ip::address_v4::from_string(ip), port),
    m_socket(io_service),
    m_retry_timer(io_service)
{
  m_retry = false;
  m_stopped = false;
  m_connect_in_progress = false;
  m_prefix_for_logger = fmt::format("AsioTCPConnector({}:{})", m_ip, m_port);
}

void
AsioTCPConnector::set_endpoint(uint32_t ip_address, uint16_t port)
{
  auto addr = asio::ip::make_address_v4(ip_address);
  m_ip = addr.to_string();
  m_port = port;
  m_endpoint = asio::ip::tcp::endpoint(addr, port);
  m_prefix_for_logger = fmt::format("AsioTCPConnector({}:{})", m_ip, m_port);
}

void
AsioTCPConnector::start(void)
{
  ASSERT(
    (m_connect_in_progress == false),
    "A connect already in progress. Ignoring.");

  m_connect_in_progress = true;
  if (m_retry) {
    auto now = util::steady_clock_now();
    auto tdiff = std::chrono::duration_cast<std::chrono::milliseconds>(
      now - m_last_start_ts);
    auto wait_for = m_policy->wait_for();
    if (tdiff < wait_for) {
      auto remaining_wait_for =
        std::chrono::duration_cast<std::chrono::milliseconds>(
          wait_for - tdiff);
      m_retry_timer.expires_from_now(remaining_wait_for);
      m_retry_timer.async_wait(
        std::bind(&AsioTCPConnector::start_connect, this));
      return;
    }
  }
  start_connect();
}

void
AsioTCPConnector::stop(void)
{
  m_stopped = true;
  if (m_connect_in_progress) {
    m_socket.cancel();
    m_retry_timer.cancel();
  }
}

void
AsioTCPConnector::enable_retry(
  std::unique_ptr<TCPConnectorRetryPolicy> policy)
{
  m_retry = true;
  m_policy = std::move(policy);
}

void
AsioTCPConnector::disable_retry(void)
{
  m_retry = false;
}

void
AsioTCPConnector::set_max_retry_count(
  int count, std::function<void()> max_retry_reached_cb)
{
  m_max_retry_count = count;
  m_retry_count = 0;
  m_max_retry_reached_cb = max_retry_reached_cb;
}

void
AsioTCPConnector::start_connect(void)
{
  m_last_start_ts = util::steady_clock_now();
  m_socket.async_connect(
    m_endpoint,
    std::bind(
      &AsioTCPConnector::handle_connect, this, std::placeholders::_1));
}

void
AsioTCPConnector::handle_connect(const boost::system::error_code& error)
{
  if (error) {
    handle_error(error);
    return;
  }

  if (m_stopped) {
    boost::system::error_code error;
    m_logger->warn(
      "{}: Connection established after being stopped. Dropping connection.",
      m_prefix_for_logger);
    m_socket.shutdown(ip::tcp::socket::shutdown_both, error);
    m_socket.close();
    return;
  }

  m_retry_count = 0;
  auto connection = m_connection_factory->create(std::move(m_socket));
  connection->set_connection_manager(m_connection_manager);

  m_connect_in_progress = false;
  m_connection_manager->connected(connection);
}

void
AsioTCPConnector::handle_error(const boost::system::error_code& error)
{
  m_logger->error(
    "{}: Failed to connect, error='{}'.", m_prefix_for_logger,
    error.message());

  if (m_retry == false) {
    m_logger->error("{}: Retry not enabled. Giving up.", m_prefix_for_logger);
    m_connect_in_progress = false;
    return;
  }

  if (m_stopped == true) {
    m_logger->error("{}: Stop initiated. Not retrying.", m_prefix_for_logger);
    m_connect_in_progress = false;
    return;
  }

  m_retry_count += 1;
  if ((m_max_retry_count != 0) && (m_retry_count == m_max_retry_count)) {
    m_logger->error("{}: Max retry count reached.", m_prefix_for_logger);
    m_connect_in_progress = false;
    m_max_retry_reached_cb();
    return;
  }

  auto tdiff = m_policy->wait_for();
  m_logger->error(
    "{}: Retry {}. Scheduling in {} ms.", m_prefix_for_logger, m_retry_count,
    tdiff.count());

  m_retry_timer.expires_from_now(std::chrono::milliseconds(tdiff));
  m_retry_timer.async_wait(std::bind(&AsioTCPConnector::start_connect, this));
  return;
}

NAMESPACE__NSEIT__END

/*
  Local Variables:
  mode: c++
  End:
*/
