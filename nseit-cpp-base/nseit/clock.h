/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__03306072_11cf_11ea_8f23_d43b04f4acea
#define NSEIT_CPP_BASE__03306072_11cf_11ea_8f23_d43b04f4acea

#include <chrono>

#include "_namespace.h"

NAMESPACE__NSEIT__START

class Clock
{
public:
  void set_now(std::chrono::steady_clock::time_point now)
  {
    m_now = now;
  }

  std::chrono::steady_clock::time_point now(void) const;

private:
  std::chrono::steady_clock::time_point m_now;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__03306072_11cf_11ea_8f23_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
