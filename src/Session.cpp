#include "Session.h"
#include "CommandHandler.h"
#include "Server.h"
#include <iostream>
#include <memory>

Session::Session(asio::io_context &io_context, Server *server, bool is_master)
    : socket_(io_context),
      command_handler_(server->data_, server->replication_info_, this),
      is_master_session_(is_master), replicas_(server->replica_sessions_) {}

Session::Session(tcp::socket &&socket, Server *server, bool is_master)
    : socket_(std::move(socket)),
      command_handler_(server->data_, server->replication_info_, this),
      is_master_session_(is_master), replicas_(server->replica_sessions_) {}

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

    std::cout << "Debug: receive message = " << message << "\n";

    command_handler_.handle_raw_command(message);

  } else if (error_code == asio::error::eof) {
    is_closed = true;
    std::cout << "Connection closed by peer\n";
  } else if (error_code == asio::error::operation_aborted) {
    is_closed = true;
    std::cout << "Operation aborted\n";
  } else {
    is_closed = true;
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

void Session::set_as_replica() {
  is_replica_session_ = true;
  replicas_->add_replica(shared_from_this());
}

bool Session::is_master_session() const { return is_master_session_; }

std::vector<std::shared_ptr<Session>> Session::get_replicas() const {
  return replicas_->get_replicas();
}

bool Session::is_session_closed() const {
  return is_closed;
}