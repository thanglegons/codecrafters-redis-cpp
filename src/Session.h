#pragma once

#include "CommandHandler.h"
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>
#include <memory>

using asio::ip::tcp;

class Server;
class Replicas;

class Session : public std::enable_shared_from_this<Session> {
  friend class CommandHandler;

public:
  Session(asio::io_context &io_context, Server* server, bool is_master = false);

  Session(tcp::socket&& socket, Server* server, bool is_master = false);

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

  void set_as_replica();

  bool is_master_session() const;

  std::vector<std::shared_ptr<Session>> get_replicas() const;

  bool is_session_closed() const;

private:
  void handle_read(const asio::error_code &error_code, size_t len);

  void handle_write(const asio::error_code &error_code, size_t len);

  static constexpr size_t kBufferSize = 1024;

  tcp::socket socket_;
  char data_[kBufferSize];
  CommandHandler command_handler_;
  bool is_replica_session_{false};
  bool is_master_session_{false};
  bool is_closed{false};
  std::shared_ptr<Replicas> replicas_;
};