# Copyright (C) 2016-2020 NSEIT Limited, Mumbai, India. All rights
# reserved.
#
# This program and the accompanying materials are made available under
# the terms described in the LICENSE file which accompanies this
# distribution. If the LICENSE file was not attached to this
# distribution or for further clarifications, please contact
# legal@nseit.com.

cmake_minimum_required (VERSION 2.8)
project (nseit-cpp-base)

set(CMAKE_CXX_STANDARD 14)

# Check if we are being built as a part of sps-node or sps-master or
# as isolated.
set (ISOLATED_BUILD ON)

if (NOT "${CMAKE_PROJECT_NAME}" STREQUAL "nseit-cpp-base")
  set (ISOLATED_BUILD OFF)
endif (NOT "${CMAKE_PROJECT_NAME}" STREQUAL "nseit-cpp-base")

set (Boost_USE_STATIC_LIBS ON)
find_package (Boost REQUIRED COMPONENTS
  thread date_time system program_options)
find_package (Threads)

add_subdirectory(spdlog)
add_subdirectory(nseit)

# if (ISOLATED_BUILD)
#   add_subdirectory (googletest)
#   add_subdirectory (tests)
# endif (ISOLATED_BUILD)
