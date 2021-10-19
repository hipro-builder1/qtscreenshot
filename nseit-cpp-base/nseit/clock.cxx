/* Copyright (C) 2016-2020 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#include "_namespace.h"
#include "clock.h"

#include <boost/chrono.hpp>

NAMESPACE__NSEIT__START

namespace bchrono = boost::chrono;

std::chrono::steady_clock::time_point
Clock::now() const
{
  auto b_now = bchrono::steady_clock::now();
  auto b_epoch = b_now.time_since_epoch();
  auto b_ms = bchrono::duration_cast<bchrono::milliseconds>(b_epoch);

  auto std_ms = std::chrono::milliseconds(b_ms.count());
  auto std_now = std::chrono::steady_clock::time_point(std_ms);
  return std_now;
}

NAMESPACE__NSEIT__END

/*
  Local Variables:
  mode: c++
  End:
*/
