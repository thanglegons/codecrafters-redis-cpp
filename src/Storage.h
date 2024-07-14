#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class KVStorage {
public:
  void set(std::string k, std::string v, uint32_t expiring_time_ms = -1);

  void set_with_timestamp(std::string k, std::string v, uint64_t timestamp);

  std::optional<std::string> get(const std::string &k) const;

  void update_info(std::string k, std::string v);

  std::vector<std::string> get_keys() const;

private:
  struct ExpiringValue {
    std::string value;
    uint64_t expired_ts_ms;
  };
  mutable std::unordered_map<std::string, ExpiringValue> data_;
  std::unordered_map<std::string, std::string> info_;
};