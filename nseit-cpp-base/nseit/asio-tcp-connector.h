/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__145cc5ac_edce_11e6_ba06_c85b760810f3
#define NSEIT_CPP_BASE__145cc5ac_edce_11e6_ba06_c85b760810f3

#include <chrono>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>

#include "spdlog/spdlog.h"

#include "tcp-connector.h"
#include "tcp-connection.h"
#include "tcp-connection-manager.h"
#include "tcp-connector-retry-policy.h"
#include "_namespace.h"

NAMESPACE__NSEIT__START

class AsioTCPConnector : public TCPConnector
{
public:
  AsioTCPConnector(
    std::shared_ptr<spdlog::logger> logger,
    boost::asio::io_service& io_service,
    TCPConnectionFactory* connection_factory, const std::string& ip,
    int port);
  ~AsioTCPConnector() {}

  /*--- DI ---*/
  void set_connection_manager(TCPConnectionManager* connection_manager)
  {
    m_connection_manager = connection_manager;
  }

  /*--- TCPConnector interface ---*/
  void set_endpoint(uint32_t ip_address, uint16_t port);
  void start(void);
  void stop(void);
  void enable_retry(std::unique_ptr<TCPConnectorRetryPolicy> policy);
  void disable_retry(void);
  void set_max_retry_count(
    int count, std::function<void()> max_retry_reached_cb);

private:
  void start_connect(void);
  void handle_connect(const boost::system::error_code& error);
  void handle_error(const boost::system::error_code& error);

private:
  std::shared_ptr<spdlog::logger> m_logger;
  boost::asio::io_service& m_io_service;
  TCPConnectionFactory* m_connection_factory = nullptr;

  std::string m_ip;
  int m_port;
  boost::asio::ip::tcp::endpoint m_endpoint;
  boost::asio::ip::tcp::socket m_socket;
  boost::asio::steady_timer m_retry_timer;

  TCPConnectionManager* m_connection_manager = nullptr;
  bool m_retry;
  bool m_stopped;
  bool m_connect_in_progress;
  std::unique_ptr<TCPConnectorRetryPolicy> m_policy;
  std::chrono::steady_clock::time_point m_last_start_ts;
  int m_max_retry_count = 0;
  int m_retry_count = 0;
  std::function<void()> m_max_retry_reached_cb;
  std::string m_prefix_for_logger;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__145cc5ac_edce_11e6_ba06_c85b760810f3

/*
  Local Variables:
  mode: c++
  End:
*/
