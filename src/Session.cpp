#include "Session.h"
#include "CommandHandler.h"
#include "ReplicationInfo.h"
#include <iostream>

Session::Session(asio::io_context &io_context, std::shared_ptr<KVStorage> data,
                 std::shared_ptr<ReplicationInfo> replication_info)
    : socket_(io_context),
      command_handler_(std::move(data), std::move(replication_info), this) {}

tcp::socket &Session::get_socket() { return socket_; }

void Session::start() {
  auto shared_self = shared_from_this();
  socket_.async_read_some(
      asio::buffer(data_),
      [this, shared_self](const asio::error_code &error_code, size_t len) {
        handle_read(error_code, len);
      });
}

void Session::handle_read(const asio::error_code &error_code, size_t len) {
  if (!error_code) {
    // process the received message
    std::string message(data_, len);

    command_handler_.handle_raw_command(message);

  } else if (error_code == asio::error::eof) {
    std::cout << "Connection closed by peer\n";
  } else if (error_code == asio::error::operation_aborted) {
    std::cout << "Operation aborted\n";
  } else {
    std::cout << "Error: " << error_code.message() << "\n";
  }
}

void Session::handle_write(const asio::error_code &error_code, size_t len) {
  if (!error_code) {
    start();
  } else {
    std::cout << "Read failed, error = " << error_code.message() << "\n";
  }
}