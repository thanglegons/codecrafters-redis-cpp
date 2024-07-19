#include "Storage.h"
#include <ctime>
#include <iostream>
#include <limits>
#include <optional>

std::optional<ValueType> KVStorage::get_raw(const std::string &k) const {
  auto current_time_ms = get_current_timestamp_ms();
  auto it = data_.find(k);
  if (it == data_.end()) {
    return std::nullopt;
  }
  if (it->second.expired_ts_ms <= current_time_ms) {
    data_.erase(k);
    return std::nullopt;
  }
  return it->second.value; // should be a copy
}

std::optional<std::string> KVStorage::get_string(const std::string &k) const {
  return get<std::string>(k);
}

std::optional<Stream> KVStorage::get_stream(const std::string &k) const {
  return get<Stream>(k);
}

void KVStorage::set_string(std::string k, std::string v,
                           uint32_t expiring_time_ms) {
  uint64_t expired_ts_ms = expiring_time_ms == -1
                               ? std::numeric_limits<uint64_t>::max()
                               : get_current_timestamp_ms() + expiring_time_ms;
  set_with_timestamp(std::move(k), std::move(v), expired_ts_ms);
}

std::optional<std::string> KVStorage::set_stream(
    std::string k, std::string id,
    std::vector<std::pair<std::string, std::string>> pairs,
    Stream::StreamError &err, uint32_t expiring_time_ms) {
  uint64_t expired_ts_ms = expiring_time_ms == -1
                               ? std::numeric_limits<uint64_t>::max()
                               : get_current_timestamp_ms() + expiring_time_ms;
  auto entry_id = Stream::EntryID::toEntryID(id);
  if (!entry_id.has_value()) {
    err = Stream::StreamError::entryIDParsedError;
    return std::nullopt;
  }
  Stream::Entry entry(std::move(entry_id.value()));
  for (auto &&[ik, iv] : pairs) {
    entry.inner_kv.emplace(std::move(ik), std::move(iv));
  }
  if (auto stream = get_stream(k); stream.has_value()) {
    return stream->add_entry(std::move(entry), err);
  } else {
    Stream new_stream;
    auto ret = new_stream.add_entry(std::move(entry), err);
    if (err == Stream::StreamError::OK) {
      set_with_timestamp(std::move(k), std::move(new_stream), expired_ts_ms);
      return ret;
    }
  }
  return std::nullopt;
}

void KVStorage::set_with_timestamp(std::string k, ValueType v,
                                   uint64_t expire_timestap_ms) {
  data_.insert_or_assign(std::move(k),
                         ExpiringValue{std::move(v), expire_timestap_ms});
}

void KVStorage::update_info(std::string k, std::string v) {
  info_.emplace(std::move(k), std::move(v));
}

std::vector<std::string> KVStorage::get_keys() const {
  std::vector<std::string> keys;
  for (const auto &[k, _] : data_) {
    keys.emplace_back(k);
  }
  return keys;
}
