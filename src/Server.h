#pragma once

#include "Session.h"
#include "Storage.h"
#include "ReplicationInfo.h"
#include <asio/error_code.hpp>
#include <asio/io_context.hpp>

struct ServerConfig {
  int16_t port;

  struct ReplicaOf {
    std::string host;
    int16_t port;
  };
  std::optional<ReplicaOf> replicaof;
};

class Server {
public:
  Server(asio::io_context &io_context, const ServerConfig& config);

private:
  void start_accept();

  void handle_accept(std::shared_ptr<Session> session,
                     const asio::error_code &error_code);

  std::shared_ptr<ReplicationInfo> init_replication_info(const ServerConfig& config);

  bool master_handshake(const ServerConfig&);

  tcp::acceptor acceptor_;
  asio::io_context &io_context;
  std::shared_ptr<KVStorage> data_;
  std::shared_ptr<ReplicationInfo> replication_info_;
};