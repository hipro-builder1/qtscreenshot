/* Copyright (C) 2016-2017 HiPro IT Solutions Private Limited,
 * Chennai. All rights reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@hipro.co.in. */

#ifndef NSEIT_CPP_BASE__2b575ec4_1935_11e7_92ee_448500dfb04c
#define NSEIT_CPP_BASE__2b575ec4_1935_11e7_92ee_448500dfb04c

#include <unistd.h>

#include <string>
#include <chrono>
#include <memory>

#include <boost/chrono.hpp>
#include <boost/asio/basic_waitable_timer.hpp>

#include "spdlog/spdlog.h"

#include "_namespace.h"
#include "types.h"

NAMESPACE__NSEIT_UTIL__START

typedef boost::asio::basic_waitable_timer<boost::chrono::steady_clock>
  steady_timer;

std::chrono::steady_clock::time_point steady_clock_now(void);

std::string sql_escape(const std::string& orig);
bool sql_bool(const char* val);
int sql_bool_to_int(bool val);
int sql_int(const char* val);

std::string format_mac(uint64_t mac);
std::string format_ip(uint32_t ip);

bool safe_read(
  int fd, void* buf, size_t buf_size, size_t* read_count,
  std::shared_ptr<spdlog::logger> logger);

NAMESPACE__NSEIT_UTIL__END

#endif // NSEIT_CPP_BASE__2b575ec4_1935_11e7_92ee_448500dfb04c

/*
  Local Variables:
  mode: c++
  End:
*/
