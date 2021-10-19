/* Copyright (C) 2008-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#include <stdlib.h>

#include "_namespace.h"
#include "simple-tcp-connector-retry-policy_impl.h"

NAMESPACE__NSEIT__START

SimpleTCPConnectorRetryPolicy::SimpleTCPConnectorRetryPolicy(
  const std::chrono::milliseconds& base)
  : m_base(base), m_tolerance(base.count() * 0.10)
{}

std::chrono::milliseconds
SimpleTCPConnectorRetryPolicy::wait_for(void) const
{
#if defined(WIN32) || defined(_WIN32) ||                                     \
  defined(__WIN32) && !defined(__CYGWIN__)
  auto random_val = rand();
#else
  auto random_val = random();
#endif
  auto r_median = (RAND_MAX / 2);
  auto r_relative = 0;
  if (random_val < r_median) {
    r_relative = random_val * -1;
  } else {
    r_relative = random_val - r_median;
  }
  auto r_percent = (1.0 * r_relative) / r_median;

  double backoff = m_tolerance * r_percent;
  auto timeout = static_cast<long int>(m_base.count() + backoff);
  return std::chrono::milliseconds(timeout);
}

NAMESPACE__NSEIT__END

/*
  Local Variables:
  mode: c++
  End:
*/
