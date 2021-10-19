/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__d28437c2_df89_11e9_bb66_d43b04f4acea
#define NSEIT_CPP_BASE__d28437c2_df89_11e9_bb66_d43b04f4acea

#include "_namespace.h"

NAMESPACE__NSEIT__START

class TCPListener
{
public:
  virtual ~TCPListener() {}

  virtual void start(void) = 0;
  virtual void stop(void) = 0;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__d28437c2_df89_11e9_bb66_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
