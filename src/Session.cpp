#include "Session.h"
#include <iostream>

Session::Session(asio::io_context &io_context) : socket_(io_context) {}

std::shared_ptr<Session> Session::create(asio::io_context &io_context) {
  return std::make_shared<Session>(io_context);
}

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
    std::cout << "Received (" << message << ")\n";

    std::string return_message = "+PONG\r\n";
    asio::async_write(socket_, asio::buffer(return_message),
                      [this, self = shared_from_this()](
                          const asio::error_code &error_code, size_t len) {
                        handle_write(error_code, len);
                      });
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
  }
}