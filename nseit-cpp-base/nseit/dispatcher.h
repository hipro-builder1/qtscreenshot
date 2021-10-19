/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__67d91b84_0aa1_11ea_a5d7_d43b04f4acea
#define NSEIT_CPP_BASE__67d91b84_0aa1_11ea_a5d7_d43b04f4acea

#include <functional>

#include "_namespace.h"

NAMESPACE__NSEIT__START

class Dispatcher
{
public:
  virtual ~Dispatcher() {}

  virtual void dispatch(std::function<void(void)> fn) = 0;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__67d91b84_0aa1_11ea_a5d7_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
