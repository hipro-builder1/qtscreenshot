/* Copyright (C) 2016-2018 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__d4b2cd92_c585_11e9_95c0_d43b04f4acea
#define NSEIT_CPP_BASE__d4b2cd92_c585_11e9_95c0_d43b04f4acea

#include <cstdint>

#include "msg-transport.h"
#include "_namespace.h"

NAMESPACE__NSEIT__START

class PMux : public DownstreamMsgTransport
{
public:
  virtual ~PMux() {}

  /* Method to start the pmux */
  virtual void start() = 0;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__d4b2cd92_c585_11e9_95c0_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
