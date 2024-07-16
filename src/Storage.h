#pragma once

#include "Helpers.h"
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <memory>

struct Stream {
  struct Entry {
    explicit Entry(std::string id) : id(std::move(id)) {}
    std::string id;
    std::unordered_map<std::string, std::string> inner_kv;
  };

  Stream() : entries(std::make_shared<std::vector<Entry>>()) {}

  std::shared_ptr<std::vector<Entry>> entries;

  void add_entry(Entry entry) { entries->emplace_back(std::move(entry)); }
};

using ValueType = std::variant<std::string, Stream>;

class KVStorage {
public:
  std::optional<ValueType> get_raw(const std::string &) const;

  std::optional<std::string> get_string(const std::string &k) const;

  std::optional<Stream> get_stream(const std::string &) const;

  void set_string(std::string k, std::string v, uint32_t expiring_time_ms = -1);

  void set_stream(std::string k, std::string id,
                  std::vector<std::pair<std::string, std::string>> pairs,
                  uint32_t expiring_time_ms = -1);

  void set_with_timestamp(std::string k, ValueType v, uint64_t timestamp);

  void update_info(std::string k, std::string v);

  std::vector<std::string> get_keys() const;

private:
  struct ExpiringValue {
    ValueType value;
    uint64_t expired_ts_ms;
  };
  mutable std::unordered_map<std::string, ExpiringValue> data_;
  std::unordered_map<std::string, std::string> info_;

private:
  template <typename T> std::optional<T> get(const std::string &k) const {
    auto value = get_raw(k);
    if (!value.has_value()) {
      return std::nullopt;
    }
    if (!std::holds_alternative<T>(value.value())) {
      return std::nullopt;
    }
    return std::get<T>(value.value());
  }
};