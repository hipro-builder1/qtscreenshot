/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__aaaf0348_fb0f_11e9_9012_d43b04f4acea
#define NSEIT_CPP_BASE__aaaf0348_fb0f_11e9_9012_d43b04f4acea

#include <stdexcept>

#include "spdlog/spdlog.h"

#define ASSERT(x, fmt, ...)                                                  \
  do {                                                                       \
    if (!x) {                                                                \
      spdlog::error("Assertion failed: " #x "\n" fmt, ##__VA_ARGS__);        \
      throw std::runtime_error("Assertion failure");                         \
    }                                                                        \
  } while (0)

#define THROW(x, y)                                                          \
  do {                                                                       \
    spdlog::error(y);                                                        \
    throw x(y);                                                              \
  } while (0)

#endif // NSEIT_CPP_BASE__aaaf0348_fb0f_11e9_9012_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
