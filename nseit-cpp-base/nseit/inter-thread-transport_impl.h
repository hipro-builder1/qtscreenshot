/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__2c49c5a6_e1e2_11e9_bb66_d43b04f4acea
#define NSEIT_CPP_BASE__2c49c5a6_e1e2_11e9_bb66_d43b04f4acea

#include <mutex>
#include <map>

#include "spdlog/spdlog.h"

#include "inter-thread-transport.h"
#include "msg-transport.h"
#include "buffer.h"
#include "types.h"

NAMESPACE__NSEIT__START

class InterThreadTransportImpl : public InterThreadTransport
{
private:
  enum class AtCtEventType
  {
    MESSAGE = 1,
    DROP_CONNECTION = 2,
  };

  struct AtCtEvent
  {
    AtCtEvent() {}

    AtCtEvent(AtCtEventType event_type_p, unsigned long connection_id_p)
      : event_type(event_type_p), connection_id(connection_id_p)
    {}

    AtCtEventType event_type;
    unsigned long connection_id;
    uint8_t message_type;
    Buffer message_buffer;
  };

  enum class CtAtEventType
  {
    MESSAGE = 1,
    NEW_CONNECTION = 2,
    GOT_ERROR = 3,
  };

  struct CtAtEvent
  {
    CtAtEvent() {}

    CtAtEvent(CtAtEventType event_type_p, unsigned long connection_id_p)
      : event_type(event_type_p), connection_id(connection_id_p)
    {}

    CtAtEventType event_type;
    unsigned long connection_id;
    int connection_type;
    uint8_t message_type;
    Buffer message_buffer;
    boost::system::error_code error;
    IpAddress remote_ip;
  };

public:
  InterThreadTransportImpl(
    std::shared_ptr<spdlog::logger> at_logger,
    std::shared_ptr<spdlog::logger> ct_logger);

  /*--- DI ---*/
  void set_app_dispatcher(Dispatcher* app_dispatcher)
  {
    m_app_dispatcher = app_dispatcher;
  }

  void set_com_dispatcher(Dispatcher* com_dispatcher)
  {
    m_com_dispatcher = com_dispatcher;
  }

  /*--- InterThreadTransport interface ---*/
  void attach_upstream_msg_transport_factory(
    int connection_type, UpstreamMsgTransportFactory* factory);
  void send_to_app_thread(
    unsigned long connection_id, uint8_t type, Buffer&& buffer);
  void send_to_com_thread(
    unsigned long connection_id, uint8_t type, Buffer&& buffer);
  unsigned long new_connection(
    DownstreamMsgTransport* downstream_transport, int connection_type,
    IpAddress remote_ip);
  void drop_connection(unsigned long connection_id);
  void got_error(
    unsigned long connection_id, const boost::system::error_code& error);

private:
  void wake_up_com(void);
  void wake_up_app(void);

private:
  std::shared_ptr<spdlog::logger> m_at_logger;
  std::shared_ptr<spdlog::logger> m_ct_logger;

  Dispatcher* m_app_dispatcher = nullptr;
  Dispatcher* m_com_dispatcher = nullptr;
  std::map<int, UpstreamMsgTransportFactory*> m_at_transport_factory_map;

  std::vector<AtCtEvent> m_at_ct_queue;
  std::vector<CtAtEvent> m_ct_at_queue;
  std::mutex m_at_ct_mutex;
  std::mutex m_ct_at_mutex;

  unsigned long m_last_connection_id = 0;
  std::map<unsigned long, DownstreamMsgTransport*> m_ct_map;
  std::map<unsigned long, std::unique_ptr<UpstreamMsgTransport>> m_at_map;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__2c49c5a6_e1e2_11e9_bb66_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
