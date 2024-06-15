#include "Server.h"
#include "Helpers.h"
#include <asio/io_context.hpp>
#include <iostream>
#include <memory>

Server::Server(asio::io_context &io_context, const ServerConfig& config)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), config.port)),
      io_context(io_context),
      data_(std::make_shared<KVStorage>()),
      replication_info_(init_replication_info(config)) {
  start_accept();
}

void Server::start_accept() {
  auto new_session = std::make_shared<Session>(io_context, data_, replication_info_);
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

std::shared_ptr<ReplicationInfo> Server::init_replication_info(const ServerConfig& config) {
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