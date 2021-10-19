/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__ba3f603e_0a21_11ea_a5d7_d43b04f4acea
#define NSEIT_CPP_BASE__ba3f603e_0a21_11ea_a5d7_d43b04f4acea

#include <stdexcept>
#include <memory>

#include "_namespace.h"
#include "msg-transport.h"
#include "inter-thread-transport.h"

NAMESPACE__NSEIT__START

/* Adapts the ITT interface to the DownstreamMsgTransport. */
class DownstreamMsgTransportIttAdapter : public DownstreamMsgTransport,
                                         public UpstreamMsgTransport
{
public:
  DownstreamMsgTransportIttAdapter(
    InterThreadTransport* itt, unsigned long connection_id)
    : m_itt(itt), m_connection_id(connection_id)
  {}
  /*--- configure ---*/
  void set_upstream_msg_transport(
    std::unique_ptr<UpstreamMsgTransport>&& upstream_transport)
  {
    m_upstream_transport = std::move(upstream_transport);
  }

  /*--- DownstreamMsgTransport interface ---*/
  void send_message(uint8_t type, Buffer&& buffer)
  {
    m_itt->send_to_com_thread(m_connection_id, type, std::move(buffer));
  }

  void drop_connection_from_upstream()
  {
    m_itt->drop_connection(m_connection_id);
  }

  void drop_connection()
  {
    throw std::logic_error("Talk to the business objects /)_-)");
  }

  /*--- UpstreamMsgTransport interface ---*/
  void connected()
  {
    m_upstream_transport->connected();
  }

  void got_message(uint8_t type, Buffer&& buffer)
  {
    m_upstream_transport->got_message(type, std::move(buffer));
  }

  void got_error(const boost::system::error_code& error)
  {
    m_upstream_transport->got_error(error);
  }

private:
  InterThreadTransport* m_itt = nullptr;
  std::unique_ptr<UpstreamMsgTransport> m_upstream_transport;
  unsigned long m_connection_id;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__ba3f603e_0a21_11ea_a5d7_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
