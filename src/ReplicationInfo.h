#pragma once
#include <string>

struct ReplicationInfo {
  bool is_master;

  std::string returnRawInfo() const;
};