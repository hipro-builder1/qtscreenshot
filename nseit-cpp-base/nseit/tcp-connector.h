/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__f71e7744_0211_11e7_a1c8_448500dfb04c
#define NSEIT_CPP_BASE__f71e7744_0211_11e7_a1c8_448500dfb04c

#include <memory>
#include <functional>

#include "tcp-connector-retry-policy.h"
#include "_namespace.h"

NAMESPACE__NSEIT__START

class TCPConnector
{
public:
  virtual ~TCPConnector() {}

  virtual void set_endpoint(uint32_t ip_address, uint16_t port) = 0;

  virtual void start(void) = 0;
  virtual void stop(void) = 0;

  virtual void enable_retry(
    std::unique_ptr<TCPConnectorRetryPolicy> policy) = 0;
  virtual void disable_retry(void) = 0;
  virtual void set_max_retry_count(
    int count, std::function<void()> max_retry_reached_cb) = 0;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__f71e7744_0211_11e7_a1c8_448500dfb04c

/*
  Local Variables:
  mode: c++
  End:
*/
