/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__d654ba52_df89_11e9_bb66_d43b04f4acea
#define NSEIT_CPP_BASE__d654ba52_df89_11e9_bb66_d43b04f4acea

#include <memory>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>

#include "spdlog/spdlog.h"

#include "tcp-listener.h"
#include "tcp-connection.h"
#include "tcp-connection-manager_impl.h"
#include "_namespace.h"

NAMESPACE__NSEIT__START

class AsioTCPListener : public TCPListener
{
public:
  AsioTCPListener(
    std::shared_ptr<spdlog::logger> logger,
    boost::asio::io_service& io_service,
    TCPConnectionFactory* connection_factory, const std::string& ip,
    int port);
  ~AsioTCPListener() {}

  /*--- TCPListener interface ---*/
  void start(void);
  void stop(void);

private:
  void start_accept(void);
  void handle_accept(const boost::system::error_code& error);

private:
  std::shared_ptr<spdlog::logger> m_logger;
  const std::string m_ip;
  const int m_port;
  boost::asio::ip::tcp::acceptor m_acceptor;
  TCPConnectionFactory* m_connection_factory = nullptr;

  TCPConnectionManagerImpl m_connection_manager;
  boost::asio::ip::tcp::socket m_socket;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__d654ba52_df89_11e9_bb66_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
