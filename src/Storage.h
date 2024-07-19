#pragma once

#include "Helpers.h"
#include "Stream.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>


using ValueType = std::variant<std::string, Stream>;

class KVStorage {
public:
  std::optional<ValueType> get_raw(const std::string &) const;

  std::optional<std::string> get_string(const std::string &k) const;

  std::optional<Stream> get_stream(const std::string &) const;

  void set_string(std::string k, std::string v, uint32_t expiring_time_ms = -1);

  std::optional<std::string> set_stream(std::string k, std::string id,
                  std::vector<std::pair<std::string, std::string>> pairs,
                  Stream::StreamError &err, uint32_t expiring_time_ms = -1);

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