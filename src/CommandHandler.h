#pragma once
#include "ReplicationInfo.h"
#include "Storage.h"
#include <memory>

class CommandHandler {
public:
  CommandHandler(std::shared_ptr<KVStorage> data, std::shared_ptr<ReplicationInfo> replication_info);

  std::string handle_raw_command(const std::string &raw_command);

private:
  std::shared_ptr<KVStorage> data_;
  std::shared_ptr<ReplicationInfo> replication_info_;

  inline static const std::string kErrorReturn = "$-1\r\n";
};