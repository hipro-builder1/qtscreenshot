/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__c3f7b8d6_0c67_11ea_8f23_d43b04f4acea
#define NSEIT_CPP_BASE__c3f7b8d6_0c67_11ea_8f23_d43b04f4acea

#include "spdlog/spdlog.h"

#include "tcp-connection.h"
#include "inter-thread-transport.h"
#include "periodic-updater.h"
#include "asio-tcp-connection.h"
#include "pmux_impl.h"
#include "upstream-msg-transport-itt-adapter.h"
#include "clock.h"
#include "_namespace.h"

NAMESPACE__NSEIT__START

class CtConnectionFactory : public TCPConnectionFactory
{
public:
  CtConnectionFactory(
    std::shared_ptr<spdlog::logger> logger, Clock* clock, int connection_type)
    : m_logger(logger), m_clock(clock), m_connection_type(connection_type)
  {}

  /*--- DI ---*/
  void set_itt(InterThreadTransport* itt)
  {
    m_itt = itt;
  }

  void set_periodic_updater(PeriodicUpdater* periodic_updater)
  {
    m_periodic_updater = periodic_updater;
  }

  /*--- TCPConnectionFactory interface ---*/
  std::shared_ptr<TCPConnection> create(boost::asio::ip::tcp::socket&& socket)
  {
    auto remote_ep = socket.remote_endpoint();
    auto remote_ad = remote_ep.address();
    auto remote_ip = remote_ad.to_v4().to_ulong();

    auto connection =
      std::make_shared<AsioTCPConnection>(m_logger, std::move(socket));
    auto pmux = std::make_shared<PMuxImpl>(
      m_logger, m_clock, 2, std::chrono::seconds(1),
      std::chrono::seconds(10));
    auto pmux_itt_bridge =
      std::make_shared<UpstreamMsgTransportIttAdapter>(m_itt, pmux.get());

    auto connection_id = m_itt->new_connection(
      pmux_itt_bridge.get(), m_connection_type, remote_ip);
    pmux_itt_bridge->set_connection_id(connection_id);

    connection->set_upstream_transport(pmux);

    pmux->set_upstream_transport(pmux_itt_bridge);
    pmux->set_connection(connection.get());
    pmux->set_periodic_updater(m_periodic_updater);

    m_periodic_updater->start(pmux.get());
    pmux->start();
    return connection;
  }

private:
  std::shared_ptr<spdlog::logger> m_logger;
  Clock* m_clock;
  int m_connection_type = 0;

  InterThreadTransport* m_itt = nullptr;
  PeriodicUpdater* m_periodic_updater = nullptr;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__c3f7b8d6_0c67_11ea_8f23_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
