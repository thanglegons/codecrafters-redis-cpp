#include "Server.h"
#include <asio/io_context.hpp>
#include <iostream>
#include <memory>

Server::Server(asio::io_context &io_context, int16_t port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
      io_context(io_context),
      data_(std::make_shared<KVStorage>()) {
  start_accept();
}

void Server::start_accept() {
  auto new_session = std::make_shared<Session>(io_context, data_);
  acceptor_.async_accept(new_session->get_socket(),
                         [this, new_session](const auto &error) {
                           handle_accept(new_session, error);
                         });
}

void Server::handle_accept(std::shared_ptr<Session> session,
                           const asio::error_code &error_code) {
  if (!error_code) {
    std::cout << "Connected\n";
    session->start();
  } else {
    std::cout << "Error when accept = " << error_code.message() << "\n";
  }
  start_accept();
}
