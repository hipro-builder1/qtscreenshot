/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__2b70c492_2bc3_11ea_99d8_d43b04f4acea
#define NSEIT_CPP_BASE__2b70c492_2bc3_11ea_99d8_d43b04f4acea

#include <set>

#include "_namespace.h"
#include "tcp-connection.h"
#include "tcp-connection-manager.h"
#include "tcp-connector.h"
#include "assert.h"

NAMESPACE__NSEIT__START

class TcpClientConnectionManager : public TCPConnectionManager
{
public:
  void reconnect_on_disconnect(TCPConnector* connector)
  {
    m_connector = connector;
  }

  /*--- TCPConnectionManager interface ---*/
  void connected(std::shared_ptr<TCPConnection> connection)
  {
    auto ret = m_connections.insert(connection);
    ASSERT((ret.second), "Connection is already tracked");
  }

  void disconnected(std::shared_ptr<TCPConnection> connection)
  {
    m_connections.erase(connection);
    if (m_connector != nullptr) {
      m_connector->start();
    }
  }

private:
  std::set<std::shared_ptr<TCPConnection>> m_connections;

  TCPConnector* m_connector = nullptr;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__2b70c492_2bc3_11ea_99d8_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
