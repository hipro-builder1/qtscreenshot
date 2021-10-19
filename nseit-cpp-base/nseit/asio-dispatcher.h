/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__2c868924_ef42_11e9_a20a_d43b04f4acea
#define NSEIT_CPP_BASE__2c868924_ef42_11e9_a20a_d43b04f4acea

#include <boost/asio/io_service.hpp>

#include "dispatcher.h"
#include "_namespace.h"

NAMESPACE__NSEIT__START

class AsioDispatcher : public Dispatcher
{
public:
  AsioDispatcher(boost::asio::io_service& io_service)
    : m_io_service(io_service)
  {}

  void dispatch(std::function<void(void)> fn)
  {
    m_io_service.post(fn);
  }

private:
  boost::asio::io_service& m_io_service;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__2c868924_ef42_11e9_a20a_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
