/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#include <iostream>

#include "buffer.h"
#include "util.h"
#include "inter-thread-transport_impl.h"
#include "assert.h"

NAMESPACE__NSEIT__START

InterThreadTransportImpl::InterThreadTransportImpl(
  std::shared_ptr<spdlog::logger> at_logger,
  std::shared_ptr<spdlog::logger> ct_logger)
  : m_at_logger(at_logger), m_ct_logger(ct_logger)
{}

void
InterThreadTransportImpl::attach_upstream_msg_transport_factory(
  int connection_type, UpstreamMsgTransportFactory* factory)
{
  auto result = m_at_transport_factory_map.insert({connection_type, factory});
  ASSERT(
    (result.second == true), "Connection type = {} already has a factory",
    connection_type);
}

void
InterThreadTransportImpl::send_to_app_thread(
  unsigned long connection_id, uint8_t type, Buffer&& buffer)
{
  bool need_dispatch = false;
  {
    std::lock_guard<std::mutex> lock(m_ct_at_mutex);
    need_dispatch = m_ct_at_queue.empty();
    m_ct_at_queue.emplace_back(CtAtEventType::MESSAGE, connection_id);
    auto& event = m_ct_at_queue.back();
    event.message_type = type;
    event.message_buffer = std::move(buffer);
  }

  if (need_dispatch) {
    m_app_dispatcher->dispatch(
      std::bind(&InterThreadTransportImpl::wake_up_app, this));
  }
}

void
InterThreadTransportImpl::send_to_com_thread(
  unsigned long connection_id, uint8_t type, Buffer&& buffer)
{
  bool need_dispatch = false;
  {
    std::lock_guard<std::mutex> lock(m_at_ct_mutex);
    need_dispatch = m_at_ct_queue.empty();
    m_at_ct_queue.emplace_back(AtCtEventType::MESSAGE, connection_id);
    auto& event = m_at_ct_queue.back();
    event.message_type = type;
    event.message_buffer = std::move(buffer);
  }

  if (need_dispatch) {
    m_com_dispatcher->dispatch(
      std::bind(&InterThreadTransportImpl::wake_up_com, this));
  }
}

void
InterThreadTransportImpl::wake_up_com(void)
{
  std::vector<AtCtEvent> queue;
  {
    std::lock_guard<std::mutex> lock(m_at_ct_mutex);
    if (m_at_ct_queue.empty()) {
      return;
    }
    queue = std::move(m_at_ct_queue);
  }

  for (auto& event : queue) {
    auto search = m_ct_map.find(event.connection_id);
    if (search == m_ct_map.end()) {
      m_ct_logger->trace(
        "No CT object. event_type={}, connection_id={}.",
        static_cast<int>(event.event_type), event.connection_id);
      continue;
    }
    auto transport = search->second;
    switch (event.event_type) {
    case AtCtEventType::MESSAGE:
      transport->send_message(
        event.message_type, std::move(event.message_buffer));
      break;

    case AtCtEventType::DROP_CONNECTION:
      m_ct_logger->trace(
        "CT clearing connection {}. Pending: ct-at-q {}, at-ct-q {}.",
        event.connection_id, m_ct_at_queue.size(), m_at_ct_queue.size());
      transport->drop_connection_from_upstream();
      m_ct_map.erase(event.connection_id);
      break;
    };
  }
}

void
InterThreadTransportImpl::wake_up_app(void)
{
  std::vector<CtAtEvent> queue;
  {
    std::lock_guard<std::mutex> lock(m_ct_at_mutex);
    if (m_ct_at_queue.empty()) {
      return;
    }
    queue = std::move(m_ct_at_queue);
  }

  for (auto& event : queue) {
    if (event.event_type == CtAtEventType::NEW_CONNECTION) {
      auto fsearch = m_at_transport_factory_map.find(event.connection_type);
      ASSERT(
        (fsearch != m_at_transport_factory_map.end()),
        "No AT-transport-factory for connection-type={}",
        event.connection_type);
      auto transport =
        fsearch->second->create(event.connection_id, event.remote_ip);
      auto transport_raw = transport.get();
      auto result = m_at_map.insert(
        std::make_pair(event.connection_id, std::move(transport)));
      ASSERT(
        (result.second == true),
        "CT is generating duplicate communication ids ({})",
        event.connection_id);
      transport_raw->connected();
      continue;
    }

    auto search = m_at_map.find(event.connection_id);
    if (search == m_at_map.end()) {
      m_at_logger->trace(
        "No AT object. event_type={}, connection_id={}.",
        static_cast<int>(event.event_type), event.connection_id);
      continue;
    }
    auto& transport = search->second;
    switch (event.event_type) {
    case CtAtEventType::MESSAGE:
      transport->got_message(
        event.message_type, std::move(event.message_buffer));
      break;

    case CtAtEventType::GOT_ERROR:
      transport->got_error(event.error);
      m_at_map.erase(event.connection_id);
      m_at_logger->trace(
        "AT clearing connection {}. Pending: ct-at-q {}, at-ct-q {}.",
        event.connection_id, m_ct_at_queue.size(), m_at_ct_queue.size());
      break;

    case CtAtEventType::NEW_CONNECTION:
      // Just keeping the compiler happy. Should not get here
      // anyway.
      break;
    };
  }
}

unsigned long
InterThreadTransportImpl::new_connection(
  DownstreamMsgTransport* downstream_transport, int connection_type,
  IpAddress remote_ip)
{
  m_last_connection_id += 1;
  auto result = m_ct_map.insert({m_last_connection_id, downstream_transport});
  ASSERT(
    (result.second == true), "Duplicate connection-id on new-connection ({})",
    m_last_connection_id);

  bool need_dispatch = false;
  {
    std::lock_guard<std::mutex> lock(m_ct_at_mutex);
    need_dispatch = m_ct_at_queue.empty();
    m_ct_at_queue.emplace_back(
      CtAtEventType::NEW_CONNECTION, m_last_connection_id);
    auto& event = m_ct_at_queue.back();
    event.connection_type = connection_type;
    event.remote_ip = remote_ip;
  }

  if (need_dispatch) {
    m_app_dispatcher->dispatch(
      std::bind(&InterThreadTransportImpl::wake_up_app, this));
  }
  return m_last_connection_id;
}

void
InterThreadTransportImpl::drop_connection(unsigned long connection_id)
{
  bool need_dispatch = false;
  {
    std::lock_guard<std::mutex> lock(m_at_ct_mutex);
    need_dispatch = m_at_ct_queue.empty();
    m_at_ct_queue.emplace_back(AtCtEventType::DROP_CONNECTION, connection_id);
  }

  if (need_dispatch) {
    m_com_dispatcher->dispatch(
      std::bind(&InterThreadTransportImpl::wake_up_com, this));
  }

  m_at_map.erase(connection_id);
}

void
InterThreadTransportImpl::got_error(
  unsigned long connection_id, const boost::system::error_code& error)
{
  bool need_dispatch = false;
  {
    std::lock_guard<std::mutex> lock(m_ct_at_mutex);
    need_dispatch = m_ct_at_queue.empty();
    m_ct_at_queue.emplace_back(CtAtEventType::GOT_ERROR, connection_id);
    auto& event = m_ct_at_queue.back();
    event.error = error;
  }

  if (need_dispatch) {
    m_app_dispatcher->dispatch(
      std::bind(&InterThreadTransportImpl::wake_up_app, this));
  }

  m_ct_map.erase(connection_id);
}

NAMESPACE__NSEIT__END

/*
  Local Variables:
  mode: c++
  End:
*/
