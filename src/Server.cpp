#include "Server.h"
#include "Helpers.h"
#include "Parser.h"
#include <asio/completion_condition.hpp>
#include <asio/connect.hpp>
#include <asio/error_code.hpp>
#include <asio/io_context.hpp>
#include <asio/registered_buffer.hpp>
#include <asio/write.hpp>
#include <asio/read.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>

Server::Server(asio::io_context &io_context, const ServerConfig &config)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), config.port)),
      io_context(io_context), data_(std::make_shared<KVStorage>()),
      replication_info_(init_replication_info(config)) {
  if (!replication_info_->is_master && !master_handshake(config)) {
    throw std::runtime_error("Can't connect to master");
  }
  start_accept();
}

void Server::start_accept() {
  auto new_session =
      std::make_shared<Session>(io_context, data_, replication_info_);
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

std::shared_ptr<ReplicationInfo>
Server::init_replication_info(const ServerConfig &config) {
  static constexpr int kReplidLen = 40;
  auto info = std::make_shared<ReplicationInfo>();
  if (config.replicaof.has_value()) {
    info->is_master = false;
  } else {
    info->is_master = true;
  }
  info->master_repl_offset = 0;
  info->master_replid = random_string(kReplidLen);
  return info;
}

bool Server::master_handshake(const ServerConfig &config) {
  tcp::socket socket(io_context);
  tcp::resolver resolver(io_context);
  auto endpoint = resolver.resolve(config.replicaof->host,
                                   std::to_string(config.replicaof->port));

  asio::error_code ec;
  asio::connect(socket, endpoint, ec);

  if (!ec) {
    // step1: send PING to master
    // step2: send REPLCONF listening-port <port>
    // step3: send REPLCONF capa sync2
    std::vector<std::vector<std::string>> commands = {
        {"PING"},
        {"REPLCONF", "listening-port", std::to_string(config.port)},
        {"REPLCONF", "capa", "sync2"}};
    for (const auto &command : commands) {
      std::string message = Parser::encodeRespArray(command);
      asio::error_code command_ec;
      asio::write(socket, asio::buffer(message), command_ec);
      if (command_ec) {
        std::cout << "Failed to send command to master, error = " << command_ec.message() << "\n";
        return false;
      }
      char reply[128];
      socket.read_some(asio::buffer(reply));
    }
  } else {
    std::cout << "Failed to connect to master server\n";
    return false;
  }
  return true;
}