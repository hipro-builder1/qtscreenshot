/* Copyright (C) 2008-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__23a41e18_eebc_11e6_a94d_448500dfb04c
#define NSEIT_CPP_BASE__23a41e18_eebc_11e6_a94d_448500dfb04c

#include <chrono>

#include "tcp-connector-retry-policy.h"
#include "_namespace.h"

NAMESPACE__NSEIT__START

class SimpleTCPConnectorRetryPolicy : public TCPConnectorRetryPolicy
{
public:
  SimpleTCPConnectorRetryPolicy(const std::chrono::milliseconds& base);
  ~SimpleTCPConnectorRetryPolicy() {}

  std::chrono::milliseconds wait_for(void) const;

private:
  const std::chrono::milliseconds m_base;
  double m_tolerance;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__23a41e18_eebc_11e6_a94d_448500dfb04c

/*
  Local Variables:
  mode: c++
  End:
*/
