#pragma once

#include "ReplicationInfo.h"
#include "Storage.h"
#include <asio/error_code.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <mutex>
#include <shared_mutex>

using asio::ip::tcp;

class Session;

struct ServerConfig {
  int16_t port;

  struct ReplicaOf {
    std::string host;
    int16_t port;
  };
  std::optional<ReplicaOf> replicaof;
};

struct Replicas {
public:
  void add_replica(std::shared_ptr<Session> replica) {
    std::unique_lock<std::shared_mutex> l(mtx);
    replicas.emplace_back(std::move(replica));
  }

  std::vector<std::shared_ptr<Session>> get_replicas() const{
    std::shared_lock<std::shared_mutex> l(mtx);
    return replicas;
  }

private:
  std::vector<std::shared_ptr<Session>> replicas{};
  mutable std::shared_mutex mtx;
};

class Server {
  friend class Session;

public:
  Server(asio::io_context &io_context, const ServerConfig &config);

private:
  void start_accept();

  void handle_accept(std::shared_ptr<Session> session,
                     const asio::error_code &error_code);

  std::shared_ptr<ReplicationInfo>
  init_replication_info(const ServerConfig &config);

  bool master_handshake(const ServerConfig &);

  tcp::acceptor acceptor_;
  asio::io_context &io_context;
  std::shared_ptr<KVStorage> data_;
  std::shared_ptr<ReplicationInfo> replication_info_;
  std::shared_ptr<Replicas> replica_sessions_;
  std::shared_ptr<Session> master_session_;
};