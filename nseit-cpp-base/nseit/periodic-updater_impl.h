/* Copyright (C) 2016-2017 HiPro IT Solutions Private Limited,
 * Chennai. All rights reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@hipro.co.in. */

#ifndef NSEIT_CPP_BASE__5c671d16_04ad_11e7_aaf6_c85b760810f3
#define NSEIT_CPP_BASE__5c671d16_04ad_11e7_aaf6_c85b760810f3

#include <list>

#include "spdlog/spdlog.h"

#include "periodic-updater.h"

NAMESPACE__NSEIT__START

class PeriodicUpdaterImpl : public PeriodicUpdater
{
public:
  PeriodicUpdaterImpl(std::shared_ptr<spdlog::logger> logger);
  ~PeriodicUpdaterImpl() {}

  /*--- PeriodicUpdater interface ---*/
  void start(PeriodicUpdateReceiver* protocol);
  void stop(PeriodicUpdateReceiver* protocol);
  void periodic_update(const std::chrono::steady_clock::time_point& now);

private:
  std::shared_ptr<spdlog::logger> m_logger;

  std::list<PeriodicUpdateReceiver*> m_receivers;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__5c671d16_04ad_11e7_aaf6_c85b760810f3

/*
  Local Variables:
  mode: c++
  End:
*/
