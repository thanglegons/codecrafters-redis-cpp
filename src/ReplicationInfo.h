#pragma once
#include <cstdint>
#include <string>

struct ReplicationInfo {
  bool is_master;
  std::string master_replid;
  uint32_t master_repl_offset;

  std::string returnRawInfo() const;
};