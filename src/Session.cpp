#include "Session.h"
#include "Parser.h"
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

    auto commands = Parser::parse(message);
    std::string return_message = "+DEFAULT\r\n";
    if (!commands.empty() && commands[0] == "ping") {
      return_message = "+PONG\r\n";
    } else if (commands.size() == 2 && commands[0] == "echo") {
      return_message = "+" + commands[1] + "\r\n";
    }
    std::copy(return_message.begin(), return_message.end(), data_);

    asio::async_write(socket_, asio::buffer(data_, return_message.size()),
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
  } else {
    std::cout << "Read failed, error = " << error_code.message() << "\n";
  }
}