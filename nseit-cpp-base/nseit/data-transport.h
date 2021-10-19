/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__ff5873ac_e8fe_11e9_9b21_d43b04f4acea
#define NSEIT_CPP_BASE__ff5873ac_e8fe_11e9_9b21_d43b04f4acea

#include <cstdint>
#include <memory>
#include <deque>

#include <boost/system/error_code.hpp>

#include "buffer.h"
#include "_namespace.h"

NAMESPACE__NSEIT__START

/* Upstream transport */
class UpstreamDataTransport
{
public:
  virtual ~UpstreamDataTransport(){};

  /* Methods to recieve data and error from a down-stream transport */
  virtual void got_data(const uint8_t* data, size_t length) = 0;

  /* After the DownstreamDataTransport has initiated this, it will not
   * expect any incoming calls. i.e. it could cease to exist. */
  virtual void got_error(const boost::system::error_code& error) = 0;
};

/* Downstream transport */
class DownstreamDataTransport
{
public:
  virtual ~DownstreamDataTransport(){};

  /* Method to send data. Called by the up-stream transport. */
  virtual void send(Buffer&& data) = 0;

  /* A drop-connection request can happen at two ends. It could come
   * from the application object or from upstream. These two are
   * significant. L::drop_connection_from_upstream() need not
   * propagate a Ln+1::got_error(). L::drop_connection() should
   * propagate a Ln+1::got_error(). For both cases, we need to do a
   * Ln-1::drop_connection_from_upstream() */
  virtual void drop_connection_from_upstream() = 0;
  virtual void drop_connection() = 0;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__ff5873ac_e8fe_11e9_9b21_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
