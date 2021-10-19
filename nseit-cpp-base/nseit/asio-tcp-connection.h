/* Copyright (C) 2016-2019 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__d633089e_ac5c_11e5_adb9_d5dc84b8f444
#define NSEIT_CPP_BASE__d633089e_ac5c_11e5_adb9_d5dc84b8f444

#include <queue>
#include <memory>

#include <boost/asio/ip/tcp.hpp>

#include "spdlog/spdlog.h"

#include "tcp-connection.h"
#include "tcp-connection-manager.h"
#include "data-transport.h"
#include "_namespace.h"

NAMESPACE__NSEIT__START

class AsioTCPConnection
  : public TCPConnection,
    public ::std::enable_shared_from_this<AsioTCPConnection>

{
public:
  /*--- Our public methods ---*/
  AsioTCPConnection(
    std::shared_ptr<spdlog::logger> logger,
    boost::asio::ip::tcp::socket&& socket);
  ~AsioTCPConnection() {}

  void set_upstream_transport(
    std::shared_ptr<UpstreamDataTransport> transport)
  {
    m_upstream_transport = transport;
  }

  void set_connection_manager(TCPConnectionManager* manager)
  {
    m_connection_manager = manager;
  }

  /*--- Methods required by the TCPConnectionInterface ---*/
  void add_to_write_queue(Buffer&& buffer);
  void start_writing(void);
  void start_reading(void);
  void drop_connection_from_upstream();
  void drop_connection();

  /*--- AsioTCPConnection interface ---*/
  void print_queue_statistics(void);

private:
  /*--- Other convenience methods ---*/
  void read_completion_handler(
    const boost::system::error_code& ec, std::size_t bytes_transferred);
  void write_completion_handler(
    const boost::system::error_code& ec, std::size_t bytes_transferred);
  void do_drop_connection(void);

private:
  /*--- Data members ---*/
  std::shared_ptr<spdlog::logger> m_logger;
  boost::asio::ip::tcp::socket m_socket;

  std::shared_ptr<UpstreamDataTransport> m_upstream_transport;
  TCPConnectionManager* m_connection_manager = nullptr;

  static constexpr size_t INCOMING_BUFFER_SIZE = 8 * 1024;
  uint8_t m_incoming_buffer[INCOMING_BUFFER_SIZE];

  std::vector<Buffer> m_write_queue;
  bool m_write_in_progress = false;
  bool m_connection_dropped = false;

  std::vector<Buffer> m_write_in_progress_queue;
  std::vector<boost::asio::const_buffer> m_boost_buffers;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__d633089e_ac5c_11e5_adb9_d5dc84b8f444

/*
  Local Variables:
  mode: c++
  End:
*/
