/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__90290498_e100_11e9_bb66_d43b04f4acea
#define NSEIT_CPP_BASE__90290498_e100_11e9_bb66_d43b04f4acea

#include <set>

#include "tcp-connection-manager.h"
#include "assert.h"
#include "_namespace.h"

NAMESPACE__NSEIT__START

class TCPConnectionManagerImpl : public TCPConnectionManager
{
public:
  TCPConnectionManagerImpl() {}
  ~TCPConnectionManagerImpl() {}

  void connected(std::shared_ptr<TCPConnection> connection)
  {
    auto ret = m_connections.insert(connection);
    ASSERT((ret.second), "Connection is already tracked");
  }

  void disconnected(std::shared_ptr<TCPConnection> connection)
  {
    m_connections.erase(connection);
  }

private:
  std::set<std::shared_ptr<TCPConnection>> m_connections;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__90290498_e100_11e9_bb66_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
