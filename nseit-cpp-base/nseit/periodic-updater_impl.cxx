/* Copyright (C) 2016-2017 HiPro IT Solutions Private Limited,
 * Chennai. All rights reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@hipro.co.in. */

#include "periodic-updater_impl.h"
#include "assert.h"

NAMESPACE__NSEIT__START

PeriodicUpdaterImpl::PeriodicUpdaterImpl(
  std::shared_ptr<spdlog::logger> logger)
  : m_logger(logger)
{}

void
PeriodicUpdaterImpl::start(PeriodicUpdateReceiver* protocol)
{
  auto it = std::find(m_receivers.begin(), m_receivers.end(), protocol);
  ASSERT(
    (it == m_receivers.end()), "PeriodicUpdateReceiver registered twice");
  m_receivers.push_back(protocol);
}

void
PeriodicUpdaterImpl::stop(PeriodicUpdateReceiver* protocol)
{
  auto it = std::find(m_receivers.begin(), m_receivers.end(), protocol);
  ASSERT((it != m_receivers.end()), "PeriodicUpdateReceiver not registered");
  m_receivers.erase(it);
}

void
PeriodicUpdaterImpl::periodic_update(
  const std::chrono::steady_clock::time_point& now)
{
  auto next = m_receivers.begin();
  while (next != m_receivers.end()) {
    auto current = next;
    ++next;
    (*current)->periodic_update(now);
  }
}

NAMESPACE__NSEIT__END
/*
  Local Variables:
  mode: c++
  End:
*/
