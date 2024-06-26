#pragma once
#include "ReplicationInfo.h"
#include "Storage.h"
#include <memory>

// forward declaration
class Session;

class CommandHandler {
public:
  CommandHandler(std::shared_ptr<KVStorage> data, std::shared_ptr<ReplicationInfo> replication_info, Session* session);

  void handle_raw_command(const std::string &raw_command);

private:
  std::shared_ptr<KVStorage> data_;
  std::shared_ptr<ReplicationInfo> replication_info_;
  Session* session_;

};