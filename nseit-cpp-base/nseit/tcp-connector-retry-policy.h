/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__1f29eb38_eebc_11e6_8cbb_448500dfb04c
#define NSEIT_CPP_BASE__1f29eb38_eebc_11e6_8cbb_448500dfb04c

#include <chrono>

#include "_namespace.h"

NAMESPACE__NSEIT__START

class TCPConnectorRetryPolicy
{
public:
  virtual ~TCPConnectorRetryPolicy() {}

  virtual std::chrono::milliseconds wait_for(void) const = 0;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__1f29eb38_eebc_11e6_8cbb_448500dfb04c

/*
  Local Variables:
  mode: c++
  End:
*/
