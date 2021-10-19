/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#include <functional>
#include <chrono>

#include "spdlog/spdlog.h"

#include "assert.h"
#include "util.h"
#include "asio-tcp-listener.h"

namespace asio = ::boost::asio;

NAMESPACE__NSEIT__START

AsioTCPListener::AsioTCPListener(
  std::shared_ptr<spdlog::logger> logger, asio::io_service& io_service,
  TCPConnectionFactory* connection_factory, const std::string& ip, int port)
  : m_logger(logger),
    m_ip(ip),
    m_port(port),
    m_acceptor(
      io_service,
      asio::ip::tcp::endpoint(asio::ip::address_v4::from_string(ip), port)),
    m_connection_factory(connection_factory),
    m_connection_manager(),
    m_socket(io_service)
{}

void
AsioTCPListener::start(void)
{
  start_accept();
}

void
AsioTCPListener::stop(void)
{
  // todo: implement this
}

void
AsioTCPListener::start_accept(void)
{
  m_acceptor.async_accept(
    m_socket,
    std::bind(&AsioTCPListener::handle_accept, this, std::placeholders::_1));
}

void
AsioTCPListener::handle_accept(const boost::system::error_code& error)
{
  if (error) {
    m_logger->error(
      "async_accept({}:{}), not listening any more", m_ip, m_port,
      error.message());
    return;
  }

  auto connection = m_connection_factory->create(std::move(m_socket));
  m_connection_manager.connected(connection);
  connection->set_connection_manager(&m_connection_manager);

  start_accept();
}

NAMESPACE__NSEIT__END

/*
  Local Variables:
  mode: c++
  End:
*/
