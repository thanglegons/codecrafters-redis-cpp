#pragma once

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>

using asio::ip::tcp;
class Session : public std::enable_shared_from_this<Session> {
public:
  static std::shared_ptr<Session> create(asio::io_context &io_context); 
  
  Session(asio::io_context &io_context);

  tcp::socket &get_socket();

  void start();

private:

  void handle_read(const asio::error_code &error_code, size_t len);

  void handle_write(const asio::error_code &error_code, size_t len);

  static constexpr size_t kBufferSize = 1024;

  tcp::socket socket_;
  char data_[kBufferSize];
};