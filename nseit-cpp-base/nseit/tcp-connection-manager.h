/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__06575d94_df86_11e9_bb66_d43b04f4acea
#define NSEIT_CPP_BASE__06575d94_df86_11e9_bb66_d43b04f4acea

#include "tcp-connection.h"

#include "_namespace.h"

NAMESPACE__NSEIT__START

class TCPConnectionManager
{
public:
  virtual ~TCPConnectionManager() {}

  virtual void connected(std::shared_ptr<TCPConnection> connection) = 0;
  virtual void disconnected(std::shared_ptr<TCPConnection> connection) = 0;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__06575d94_df86_11e9_bb66_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
