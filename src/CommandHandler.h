#pragma once
#include "ReplicationInfo.h"
#include "Storage.h"
#include "commands/Command.h"
#include <memory>

// forward declaration
class Session;
class ServerConfig;

class CommandHandler {
public:
  CommandHandler(std::shared_ptr<KVStorage> data,
                 std::shared_ptr<ReplicationInfo> replication_info,
                 std::shared_ptr<ServerConfig> server_config, Session *session);

  void handle_raw_command(const std::string &raw_command);

private:
  std::shared_ptr<KVStorage> data_;
  std::shared_ptr<ReplicationInfo> replication_info_;
  std::shared_ptr<ServerConfig> server_config_;
  Session *session_;

  std::unordered_map<std::string, std::unique_ptr<commands::Command>>
      command_map_;
};