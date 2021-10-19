/* Copyright (C) 2016-2017 HiPro IT Solutions Private Limited,
 * Chennai. All rights reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@hipro.co.in. */

#include "error.h"

NAMESPACE__NSEIT_ERROR__START

namespace
{
Category category_instance;
} // namespace

const boost::system::error_category&
category(void)
{
  return category_instance;
}

NAMESPACE__NSEIT_ERROR__END

/*
  Local Variables:
  mode: c++
  End:
*/
