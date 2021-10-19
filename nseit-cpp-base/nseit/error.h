/* Copyright (C) 2016-2017 HiPro IT Solutions Private Limited,
 * Chennai. All rights reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@hipro.co.in. */

#ifndef NSEIT_CPP_BASE__4d8aa370_fc98_11e6_94cd_448500dfb04c
#define NSEIT_CPP_BASE__4d8aa370_fc98_11e6_94cd_448500dfb04c

#include <string>

#include <boost/system/error_code.hpp>

#include "_namespace.h"

NAMESPACE__NSEIT_ERROR__START

enum class Code : int
{
  APPLICATION_INITIATED_DISCONNECT = 1,
  HEARTBEAT_TIMEOUT_REACHED = 2,
  PMUX_PROTOCOL_ERROR = 3,
  INVALID_PMUX_MESSAGE_TYPE = 4,
};

class Category : public boost::system::error_category
{
public:
  const char* name() const noexcept
  {
    return "jjt::common";
  }

  std::string message(int ev) const
  {
    auto code = static_cast<Code>(ev);
    switch (code) {
    case Code::APPLICATION_INITIATED_DISCONNECT:
      return "APPLICATION_INITIATED_DISCONNECT";

    case Code::HEARTBEAT_TIMEOUT_REACHED:
      return "HEARTBEAT_TIMEOUT_REACHED";

    case Code::PMUX_PROTOCOL_ERROR:
      return "PMUX_PROTOCOL_ERROR";

    case Code::INVALID_PMUX_MESSAGE_TYPE:
      return "INVALID_PMUX_MESSAGE_TYPE";

    default:
      return "Unknown error";
    };
  }
};

const boost::system::error_category& category(void);

inline boost::system::error_code
make_error_code(error::Code code)
{
  return boost::system::error_code(static_cast<int>(code), error::category());
}

NAMESPACE__NSEIT_ERROR__END

#endif // NSEIT_CPP_BASE__4d8aa370_fc98_11e6_94cd_448500dfb04c

/*
  Local Variables:
  mode: c++
  End:
*/
