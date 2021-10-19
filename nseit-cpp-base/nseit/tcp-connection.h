/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__6127a6bc_d3c3_11e9_a296_d43b04f4acea
#define NSEIT_CPP_BASE__6127a6bc_d3c3_11e9_a296_d43b04f4acea

#include <chrono>
#include <memory>

#include <boost/asio/ip/tcp.hpp>

#include "buffer.h"
#include "_namespace.h"

NAMESPACE__NSEIT__START

class TCPConnectionManager;

class TCPConnection
{
public:
  virtual ~TCPConnection() {}

  virtual void set_connection_manager(
    TCPConnectionManager* connection_manager) = 0;

  virtual void add_to_write_queue(Buffer&& buffer) = 0;
  virtual void start_writing(void) = 0;
  virtual void start_reading(void) = 0;

  virtual void drop_connection_from_upstream() = 0;
  virtual void drop_connection() = 0;
};

class TCPConnectionFactory
{
public:
  virtual ~TCPConnectionFactory() {}

  virtual std::shared_ptr<TCPConnection> create(
    boost::asio::ip::tcp::socket&& socket) = 0;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__6127a6bc_d3c3_11e9_a296_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
