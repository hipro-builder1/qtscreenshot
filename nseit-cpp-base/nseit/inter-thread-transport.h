/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__2afa7ba0_e1e2_11e9_bb66_d43b04f4acea
#define NSEIT_CPP_BASE__2afa7ba0_e1e2_11e9_bb66_d43b04f4acea

#include <functional>

#include "msg-transport.h"
#include "dispatcher.h"
#include "_namespace.h"
#include "types.h"

NAMESPACE__NSEIT__START

class UpstreamMsgTransportFactory
{
public:
  virtual ~UpstreamMsgTransportFactory() {}

  virtual std::unique_ptr<UpstreamMsgTransport> create(
    unsigned long connection_id, IpAddress remote_ip) = 0;
};

/* InterThreadTransport
 * ====================
 *
 * The InterThreadTransport transports messages between the
 * communication-thread (CT) and the application-thread (AT). It
 * maintains two shared-queues between the CT and the AT to transfer
 * data between the two threads.
 *
 * Everytime a new connection comes in, the CT creates a new
 * connection-id to handle that connection, by calling
 * new_connection(). The InterThreadTransport generates a unqiue
 * connection-id for each connection handled by the CT. On the AT, the
 * InterThreadTransport uses the AppThreadTransportFactory to create
 * an AppThreadTransport object to handle the new connection.
 *
 * When a disconnect happens in the AT, it notifies the
 * InterThreadTransport by calling drop_connection(). The
 * InterThreadTransport then calls
 * ComThreadTransport::drop_connection_from_at() on the CT for the
 * corresponding connection-id.
 *
 * Connection drop happening at the CT end are notified via the
 * got_error().
 */

class InterThreadTransport
{
public:
  virtual ~InterThreadTransport() {}

  /* Method to attach AT connection factory. For each CT side
   * listener, we should have a unique connection type. This
   * connection-type is used to identify the at-connection-factory to
   * use for that particular incoming connection */
  virtual void attach_upstream_msg_transport_factory(
    int connection_type, UpstreamMsgTransportFactory* factory) = 0;

  /* Methods to push messages between the app-thread and the
   * com-thread */
  virtual void send_to_app_thread(
    unsigned long connection_id, uint8_t type, Buffer&& buffer) = 0;
  virtual void send_to_com_thread(
    unsigned long connection_id, uint8_t type, Buffer&& buffer) = 0;

  virtual unsigned long new_connection(
    DownstreamMsgTransport* downstream_transport, int connection_type,
    IpAddress remote_ip) = 0;
  virtual void drop_connection(unsigned long connection_id) = 0;

  virtual void got_error(
    unsigned long connection_id, const boost::system::error_code& error) = 0;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__2afa7ba0_e1e2_11e9_bb66_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
