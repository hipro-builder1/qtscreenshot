/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__f5a61edc_e5ce_11e9_bd4a_d43b04f4acea
#define NSEIT_CPP_BASE__f5a61edc_e5ce_11e9_bd4a_d43b04f4acea

#include <stdexcept>

#include "inter-thread-transport.h"
#include "msg-transport.h"
#include "_namespace.h"

NAMESPACE__NSEIT__START

/* Adapts the ITT interface to the UpstreamMsgTransport */
class UpstreamMsgTransportIttAdapter : public DownstreamMsgTransport,
                                       public UpstreamMsgTransport
{
public:
  UpstreamMsgTransportIttAdapter(
    InterThreadTransport* itt, DownstreamMsgTransport* downstream_transport)
    : m_itt(itt), m_downstream_transport(downstream_transport)
  {}

  /*--- Configure ---*/
  void set_connection_id(unsigned long connection_id)
  {
    m_connection_id = connection_id;
  }

  /*--- DownstreamMsgTransport interface ---*/
  void send_message(uint8_t type, Buffer&& buffer)
  {
    m_downstream_transport->send_message(type, std::move(buffer));
  }

  void drop_connection_from_upstream()
  {
    m_downstream_transport->drop_connection_from_upstream();
  }

  void drop_connection()
  {
    throw std::logic_error("Talk to the business objects /)_-)");
  }

  /*--- UptreamMsgTransport interface ---*/
  void connected() {}

  void got_message(uint8_t type, Buffer&& data)
  {
    m_itt->send_to_app_thread(m_connection_id, type, std::move(data));
  }

  void got_error(const boost::system::error_code& error)
  {
    m_itt->got_error(m_connection_id, error);
  }

private:
  InterThreadTransport* m_itt = nullptr;
  DownstreamMsgTransport* m_downstream_transport = nullptr;
  unsigned long m_connection_id = 0U;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__f5a61edc_e5ce_11e9_bd4a_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
