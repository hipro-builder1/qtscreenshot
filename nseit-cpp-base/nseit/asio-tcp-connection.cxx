/* Copyright (C) 2008-2017 HiPro IT Solutions Private Limited,
 * Chennai. All rights reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@hipro.co.in. */

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

#include "util.h"
#include "error.h"
#include "assert.h"
#include "asio-tcp-connection.h"

namespace bsys = ::boost::system;
namespace asio = ::boost::asio;

NAMESPACE__NSEIT__START

AsioTCPConnection::AsioTCPConnection(
  std::shared_ptr<spdlog::logger> logger, asio::ip::tcp::socket&& socket)
  : m_logger(logger), m_socket(std::move(socket))
{
  m_write_in_progress = false;
  m_connection_dropped = false;
}

void
AsioTCPConnection::add_to_write_queue(Buffer&& buffer)
{
  if (m_connection_dropped) {
    /* This could happen when a "error notification to the
     * write-handler" causes another "send". */
    m_logger->warn(
      "Connection is being dropped. Ignoring add_to_write_queue(data).");
    return;
  }

  m_write_queue.push_back(std::move(buffer));
}

void
AsioTCPConnection::start_writing(void)
{
  /* If a write is already in progress, we dont have to queue
   * another. It'll automatcially empty the m_write_queue on the next
   * run. */
  if (m_write_in_progress == true) {
    return;
  }

  /* Clear the in-progress queue */
  m_boost_buffers.clear();
  m_write_in_progress_queue.clear();

  /* If nothing is waiting to be written, we dont have to queue
   * anything. */
  if (m_write_queue.empty() == true) {
    return;
  }

  /* Populate in-progress queues and initiate a write */
  m_write_in_progress = true;
  m_write_in_progress_queue = std::move(m_write_queue);
  for (auto& buffer : m_write_in_progress_queue) {
    m_boost_buffers.push_back(
      boost::asio::buffer(buffer.pointer(), buffer.length()));
  }

  asio::async_write(
    m_socket, m_boost_buffers,
    std::bind(
      &AsioTCPConnection::write_completion_handler, shared_from_this(),
      std::placeholders::_1, std::placeholders::_2));
}

void
AsioTCPConnection::write_completion_handler(
  const bsys::error_code& ec, std::size_t bytes_transferred)
{
  m_write_in_progress = false;

  if (ec) {
    m_logger->error(
      "{}: async_write() reported error: '{}'", fmt::ptr(this), ec.message());
    /* We should notify upstream only if the connection is still
     * active. */
    if (m_connection_dropped == false) {
      m_upstream_transport->got_error(ec);
      m_connection_dropped = true;
      m_connection_manager->disconnected(shared_from_this());
    }
    m_socket.close();
    return;
  }

  /* A connection-drop has already been initiated. We should not queue
   * another write. */
  if (m_connection_dropped) {
    m_logger->warn(
      "Connection is being dropped. Not queueing another write.");
    return;
  }

  /* If there was no error, as per asio specification, all the queued
   * data should have been transferred */
  ASSERT(
    (bytes_transferred == boost::asio::buffer_size(m_boost_buffers)),
    "async_write returned without writing everything");

  /* Queue another write */
  start_writing();
}

void
AsioTCPConnection::start_reading(void)
{
  auto asio_buffer = asio::buffer(&m_incoming_buffer, INCOMING_BUFFER_SIZE);
  m_socket.async_read_some(
    asio_buffer,
    std::bind(
      &AsioTCPConnection::read_completion_handler, shared_from_this(),
      std::placeholders::_1, std::placeholders::_2));
}

void
AsioTCPConnection::read_completion_handler(
  const bsys::error_code& ec, std::size_t bytes_transferred)
{
  if (ec) {
    m_logger->error(
      "{}: async_read() reported error: '{}'", fmt::ptr(this), ec.message());
    /* We should notify upstream only if the connection is still
     * active. */
    if (m_connection_dropped == false) {
      m_upstream_transport->got_error(ec);
      m_connection_dropped = true;
      m_connection_manager->disconnected(shared_from_this());
    }
    m_socket.close();
    return;
  }

  /* A connection-drop has been scheduled. Its not done yet. In this
   * state, upstream does not care about data. So, we just ignore any
   * completion notification. */
  if (m_connection_dropped) {
    m_logger->warn(
      "Connection is being dropped. Ignoring {} incoming bytes.",
      bytes_transferred);
    return;
  }

  /* Extract data and notify upstream */
  m_upstream_transport->got_data(m_incoming_buffer, bytes_transferred);

  if (m_connection_dropped) {
    m_logger->warn("Connection is being dropped. Not queueing another read.");
    return;
  }

  start_reading();
}

void
AsioTCPConnection::drop_connection()
{
  auto error =
    error::make_error_code(error::Code::APPLICATION_INITIATED_DISCONNECT);
  m_upstream_transport->got_error(error);
  do_drop_connection();
}

void
AsioTCPConnection::drop_connection_from_upstream()
{
  do_drop_connection();
}

void
AsioTCPConnection::do_drop_connection(void)
{
  bsys::error_code error;
  m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, error);
  m_socket.close();

  m_connection_dropped = true;
  m_connection_manager->disconnected(shared_from_this());
}

void
AsioTCPConnection::print_queue_statistics(void)
{
  m_logger->debug(
    "write-queue: {}, write-in-progress-queue: {}, write-in-progress: {}",
    m_write_queue.size(), m_write_in_progress_queue.size(),
    m_write_in_progress);
}

NAMESPACE__NSEIT__END

/*
  Local Variables:
  mode: c++
  End:
*/
