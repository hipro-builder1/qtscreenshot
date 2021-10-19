/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__8dcdeed8_e8f9_11e9_9b21_d43b04f4acea
#define NSEIT_CPP_BASE__8dcdeed8_e8f9_11e9_9b21_d43b04f4acea

#include <cstdint>
#include <memory>
#include <deque>
#include <chrono>

#include <boost/system/error_code.hpp>

#include "buffer.h"
#include "_namespace.h"

NAMESPACE__NSEIT__START

class UpstreamMsgTransport
{
public:
  virtual ~UpstreamMsgTransport() {}

  virtual void connected() = 0;
  virtual void got_message(uint8_t type, Buffer&& buffer) = 0;
  virtual void got_error(
    const boost::system::error_code& error) = 0;
};

class DownstreamMsgTransport
{
public:
  virtual ~DownstreamMsgTransport() {}

  virtual void send_message(uint8_t type, Buffer&& buffer) = 0;

  /* Refer to drop-connection semantics explained in
   * data-transport.h */
  virtual void drop_connection_from_upstream() = 0;
  virtual void drop_connection() = 0;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__8dcdeed8_e8f9_11e9_9b21_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
