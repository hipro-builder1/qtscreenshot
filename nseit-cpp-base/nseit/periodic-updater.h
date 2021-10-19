/* Copyright (C) 2016-2017 HiPro IT Solutions Private Limited,
 * Chennai. All rights reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@hipro.co.in. */

#ifndef NSEIT_CPP_BASE__5a3e2c32_04ad_11e7_8e63_c85b760810f3
#define NSEIT_CPP_BASE__5a3e2c32_04ad_11e7_8e63_c85b760810f3

#include <chrono>

#include "_namespace.h"

NAMESPACE__NSEIT__START

/* Any class that wants to recieve a periodic-update should implement
 * this. */
class PeriodicUpdateReceiver
{
public:
  virtual ~PeriodicUpdateReceiver() {}

  virtual void periodic_update(
    const std::chrono::steady_clock::time_point& now) = 0;
};

/* Maintains a list of objects that require a periodic-update. Calls
 * the PeriodicUpdateReceiver::periodic_update of the registered
 * objects when ::periodic_update is called. */
class PeriodicUpdater
{
public:
  virtual ~PeriodicUpdater() {}

  /* Methods to register/unregister. */
  virtual void start(PeriodicUpdateReceiver* protocol) = 0;
  virtual void stop(PeriodicUpdateReceiver* protocol) = 0;

  /* Should be called periodically. */
  virtual void periodic_update(
    const std::chrono::steady_clock::time_point& now) = 0;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__5a3e2c32_04ad_11e7_8e63_c85b760810f3

/*
  Local Variables:
  mode: c++
  End:
*/
