#include "Server.h"
#include "Helpers.h"
#include "Parser.h"
#include "Replica.hpp"
#include "Session.h"
#include "rdb/RDB.hpp"
#include <asio/completion_condition.hpp>
#include <asio/connect.hpp>
#include <asio/error_code.hpp>
#include <asio/io_context.hpp>
#include <asio/read.hpp>
#include <asio/read_until.hpp>
#include <asio/registered_buffer.hpp>
#include <asio/streambuf.hpp>
#include <asio/write.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>

Server::Server(asio::io_context &io_context, const ServerConfig &config)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), config.port)),
      io_context(io_context), replication_info_(init_replication_info(config)),
      replica_manager_(std::make_shared<ReplicaManager>()),
      server_config_(std::make_shared<ServerConfig>(config)) {
  if (config.rdb_info.has_value()) {
    data_ = RDB::decode(config.rdb_info->dir + "/" + config.rdb_info->dbfilename);
  } else {
    data_ = std::make_shared<KVStorage>();
  }
  if (!replication_info_->is_master && !master_handshake(config)) {
    throw std::runtime_error("Can't connect to master");
  }
  start_accept();
}

void Server::start_accept() {
  auto new_session = std::make_shared<Session>(io_context, this);
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
  master_session_ = std::make_shared<Session>(io_context, this, true);
  auto &socket = master_session_->get_socket();
  tcp::resolver resolver(io_context);
  auto endpoint = resolver.resolve(config.replicaof->host,
                                   std::to_string(config.replicaof->port));

  asio::error_code ec;
  asio::connect(socket, endpoint, ec);

  if (!ec) {
    // step1: send PING to master
    // step2: send REPLCONF listening-port <port>
    // step3: send REPLCONF capa sync2
    // step4: send PSYNC ? -1
    std::vector<std::vector<std::string>> commands = {
        {"PING"},
        {"REPLCONF", "listening-port", std::to_string(config.port)},
        {"REPLCONF", "capa", "sync2"},
        {"PSYNC", "?", "-1"}};
    asio::streambuf resp_buffer;
    for (const auto &command : commands) {
      std::string message = Parser::encodeRespArray(command);
      asio::error_code command_ec;
      asio::write(socket, asio::buffer(message), command_ec);
      if (command_ec) {
        std::cout << "Failed to send command to master, error = "
                  << command_ec.message() << "\n";
        return false;
      }
      int bytes = asio::read_until(socket, resp_buffer, "\r\n");
      std::istream response_stream(&resp_buffer);
      std::string response{asio::buffers_begin(resp_buffer.data()),
                           asio::buffers_begin(resp_buffer.data()) + bytes - 2};
      resp_buffer.consume(bytes);
      std::cout << "Received: " << response << std::endl;
    }
    int num_bytes = 0;
    int bytes = asio::read_until(socket, resp_buffer, "\r\n");
    std::istream response_stream(&resp_buffer);
    std::string response{asio::buffers_begin(resp_buffer.data()),
                         asio::buffers_begin(resp_buffer.data()) + bytes - 2};
    resp_buffer.consume(bytes);
    assert(response[0] == '$');
    for (int i = 1; i < response.size(); i++) {
      num_bytes = num_bytes * 10 + (response[i] - '0');
    }
    if (resp_buffer.size() > bytes) {
      std::string rdb_content = {asio::buffers_begin(resp_buffer.data()),
                                 asio::buffers_begin(resp_buffer.data()) +
                                     num_bytes};
      resp_buffer.consume(num_bytes);
      std::string command{asio::buffers_begin(resp_buffer.data()),
                          asio::buffers_end(resp_buffer.data())};
      master_session_->start();
      master_session_->handle_message(command);
    }
  } else {
    std::cout << "Failed to connect to master server\n";
    return false;
  }
  return true;
}