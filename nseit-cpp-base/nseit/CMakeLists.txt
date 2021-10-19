# Copyright (C) 2016-2017 HiPro IT Solutions Private Limited,
# Chennai. All rights reserved.
#
# This program and the accompanying materials are made available under
# the terms described in the LICENSE file which accompanies this
# distribution. If the LICENSE file was not attached to this
# distribution or for further clarifications, please contact
# legal@hipro.co.in.

add_library (nseit STATIC
  assert.h
  buffer.cxx
  buffer.h
  clock.h
  clock.cxx
  error.cxx
  error.h
  types.h
  util.cxx
  util.h
  xxhash.cpp
  xxhash.h
  xxhash.hpp

  msg-transport.h
  data-transport.h
  dispatcher.h
  tcp-connection.h
  tcp-connector.h
  tcp-listener.h

  asio-dispatcher.h
  asio-tcp-connection.cxx
  asio-tcp-connection.h
  asio-tcp-connector.cxx
  asio-tcp-connector.h
  asio-tcp-listener.cxx
  asio-tcp-listener.h

  simple-tcp-connector-retry-policy_impl.cxx
  simple-tcp-connector-retry-policy_impl.h
  tcp-client-connection-manager.h
  tcp-connection-manager.h
  tcp-connection-manager_impl.h
  tcp-connector-retry-policy.h

  ct-connection-factory.h
  downstream-msg-transport-itt-adapter.h
  upstream-msg-transport-itt-adapter.h

  inter-thread-transport.h
  inter-thread-transport_impl.cxx
  inter-thread-transport_impl.h
  pmux.h
  pmux_impl.cxx
  pmux_impl.h

  periodic-updater.h
  periodic-updater_impl.cxx
  periodic-updater_impl.h
  )

target_include_directories (nseit INTERFACE ${PROJECT_SOURCE_DIR})

target_link_libraries (nseit
  ${Boost_LIBRARIES}
  spdlog::spdlog
  )

if(WIN32)
target_compile_options (nseit
  PRIVATE -wd4068
  )
else()
target_compile_options (nseit
  PRIVATE -Wall -Werror -Wno-unknown-pragmas
  )
endif(WIN32)
