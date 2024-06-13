#pragma once

#include "Session.h"
#include "Storage.h"
#include <asio/error_code.hpp>
#include <asio/io_context.hpp>

class Server {
public:
  Server(asio::io_context &io_context, int16_t port);

private:
  void start_accept();

  void handle_accept(std::shared_ptr<Session> session,
                     const asio::error_code &error_code);

  tcp::acceptor acceptor_;
  asio::io_context& io_context;
  std::shared_ptr<KVStorage> data_;
};