#pragma once

#include "CommandHandler.h"
#include "ReplicationInfo.h"
#include "Storage.h"
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>

using asio::ip::tcp;
class Session : public std::enable_shared_from_this<Session> {
  friend class CommandHandler;

public:
  Session(asio::io_context &io_context, std::shared_ptr<KVStorage> data,
          std::shared_ptr<ReplicationInfo> replication_info);

  tcp::socket &get_socket();

  void start();

  template <typename F> void write(std::string message, F callback) {
    std::shared_ptr<std::string> shared_message =
        std::make_shared<std::string>(std::move(message));
    asio::async_write(
        socket_, asio::buffer(*shared_message),
        [this, self = shared_from_this(), shared_message,
         callback](const asio::error_code &error_code, size_t len) {
          callback(error_code, len);
          start();
        });
  }

private:
  void handle_read(const asio::error_code &error_code, size_t len);

  void handle_write(const asio::error_code &error_code, size_t len);

  static constexpr size_t kBufferSize = 1024;

  tcp::socket socket_;
  char data_[kBufferSize];
  CommandHandler command_handler_;
};