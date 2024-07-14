#pragma once
#include <memory>

class KVStorage;

class RDB {
public:
  static std::shared_ptr<KVStorage> decode(const std::string &rdb_path);
};