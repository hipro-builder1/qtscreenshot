/* Copyright (C) 2016-2017 HiPro IT Solutions Private Limited,
 * Chennai. All rights reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@hipro.co.in. */

#include <cerrno>
#include <cstring>
#include <cstdlib>

#include "spdlog/spdlog.h"

#include "util.h"
#include "types.h"

NAMESPACE__NSEIT_UTIL__START

namespace bchrono = boost::chrono;

std::chrono::steady_clock::time_point
steady_clock_now(void)
{
  auto b_now = bchrono::steady_clock::now();
  auto b_epoch = b_now.time_since_epoch();
  auto b_ms = bchrono::duration_cast<bchrono::milliseconds>(b_epoch);

  auto std_ms = std::chrono::milliseconds(b_ms.count());
  auto std_now = std::chrono::steady_clock::time_point(std_ms);
  return std_now;
}

bool
safe_read(
  int fd, void* buf, size_t buf_size, size_t* total_read_byte,
  std::shared_ptr<spdlog::logger> logger)
{
  int count = -1;
  size_t local_read_count = 0;
  while (buf_size != 0) {
    count = read(fd, buf, buf_size);
    if (count == -1) {
      if (errno == EINTR) {
        continue;
      }
      logger->error("read failed {}", std::strerror(errno));
      return false;
    } else if (count == 0) {
      break;
    }
    buf_size = buf_size - count;
    buf = ((uint8_t*)buf) + count;
    local_read_count += count;
  }
  *total_read_byte = local_read_count;
  return true;
}

bool
sql_bool(const char* val)
{
  char* end;
  auto ival = std::strtoul(val, &end, 10);
  if (ival == 1) {
    return true;
  }
  return false;
}

int
sql_bool_to_int(bool val)
{
  if (val) {
    return 1;
  }
  return 0;
}

int
sql_int(const char* val)
{
  char* end;
  return std::strtoul(val, &end, 10);
}

std::string
sql_escape(const std::string& orig)
{
  std::ostringstream os;

  for (auto c : orig) {
    if (c == '\'') {
      os << '\'';
    }
    os << c;
  }

  return os.str();
}

std::string
format_mac(uint64_t mac)
{
  auto msg = fmt::format(
    "{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}",
    static_cast<int>((mac & 0x0000ff0000000000) >> 40),
    static_cast<int>((mac & 0x000000ff00000000) >> 32),
    static_cast<int>((mac & 0x00000000ff000000) >> 24),
    static_cast<int>((mac & 0x0000000000ff0000) >> 16),
    static_cast<int>((mac & 0x000000000000ff00) >> 8),
    static_cast<int>(mac & 0x00000000000000ff));
  return msg;
}

std::string
format_ip(uint32_t ip)
{
  auto msg = fmt::format(
    "{}.{}.{}.{}", static_cast<int>((ip & 0xff000000) >> 24),
    static_cast<int>((ip & 0x00ff0000) >> 16),
    static_cast<int>((ip & 0x0000ff00) >> 8),
    static_cast<int>(ip & 0x000000ff));
  return msg;
}

NAMESPACE__NSEIT_UTIL__END

/*
  Local Variables:
  mode: c++
  End:
*/
