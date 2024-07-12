#pragma once

#include <optional>
#include <string>

struct ServerConfig {
  int16_t port;

  struct ReplicaOf {
    std::string host;
    int16_t port;
  };
  std::optional<ReplicaOf> replicaof;

  struct RDBInfo {
    std::string dir;
    std::string dbfilename;
  };
  std::optional<RDBInfo> rdb_info;
};