#include "Stream.hpp"
#include "Helpers.h"
#include "Parser.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>

const Stream::EntryID Stream::EntryID::kZeroEntryID{0, 0};

template <>
std::string
Parser::encodeElementRespArray<Stream::Entry>(const Stream::Entry &entry) {
  std::string result = "*2\r\n";
  result += Parser::encodeBulkString(entry.id.to_string());
  result += Parser::encodeRespArray(entry.inner_kv);
  return result;
}

std::optional<Stream::EntryID>
Stream::EntryID::toEntryID(const std::string &s, int32_t default_sequence) {
  if (s == "*") {
    return std::make_optional<EntryID>(-1, -1);
  }
  auto parts = splitString(s, '-');
  if (default_sequence >= 0 && parts.size() == 1) {
    try {
      int64_t timestamp = std::stoll(parts[0]);
      return std::make_optional<EntryID>(timestamp, default_sequence);
    } catch (std::exception &e) {
      std::cout << "Failed to parse entry ID = " << s << "\n";
      return std::nullopt;
    }
  }
  if (parts.size() != 2) {
    return std::nullopt;
  }
  try {
    int64_t timestamp = std::stoll(parts[0]);
    int32_t sequence = parts[1] == "*" ? -1 : std::stoi(parts[1]);
    return std::make_optional<EntryID>(timestamp, sequence);
  } catch (std::exception &e) {
    std::cout << "Failed to parse entry ID = " << s << "\n";
    return std::nullopt;
  }
}

bool Stream::EntryID::operator<=(const EntryID &other) const {
  if (*this == other || *this < other) {
    return true;
  }
  return false;
}

bool Stream::EntryID::operator<(const EntryID &other) const {
  if (timestamp == other.timestamp) {
    return sequence < other.sequence;
  }
  return timestamp < other.timestamp;
}

std::string Stream::EntryID::to_string() const {
  return std::to_string(timestamp) + '-' + std::to_string(sequence);
}

std::optional<Stream::EntryID>
Stream::get_valid_entry_id(const Entry &new_entry, StreamError &err) const {
  auto entry_id = new_entry.id;
  if (entry_id.timestamp == -1) {
    // need auto generated for timestamp
    entry_id.timestamp = get_current_timestamp_ms();
  }
  if (entry_id.sequence == -1) {
    // need auto generated for sequence
    if (!entries->empty() &&
        entries->back().id.timestamp == entry_id.timestamp) {
      // if matched timestamp with previous entry, increase sequence by one
      entry_id.sequence = entries->back().id.sequence + 1;
    } else {
      entry_id.sequence = entry_id.timestamp == 0 ? 1 : 0;
    }
  }
  if (entry_id == Stream::EntryID::kZeroEntryID) {
    err = StreamError::entryIDIsZero;
    return std::nullopt;
  }
  if (!entries->empty() && entry_id <= entries->back().id) {
    err = StreamError::entryIDIsEqualOrSmaller;
    return std::nullopt;
  }
  return entry_id;
}

std::optional<std::string> Stream::add_entry(Entry entry, StreamError &err) {
  auto valid_entry_id = get_valid_entry_id(entry, err);
  if (!valid_entry_id.has_value()) {
    return std::nullopt;
  }
  entry.id = std::move(valid_entry_id.value());
  entries->emplace_back(std::move(entry));
  return entry.id.to_string();
}

// std::span<const Stream::Entry>
// Stream::extract_start_at(int64_t timestamp) const {
//   auto it = get_it_start_at(timestamp);
//   return std::span<const Stream::Entry>(it, entries->end());
// }

// std::span<const Stream::Entry> Stream::extract_end_at(int64_t timestamp) const {
//   auto it = get_it_start_after(timestamp);
//   return std::span<const Stream::Entry>(entries->begin(), it);
// }

std::span<const Stream::Entry>
Stream::extract_range(const std::string &raw_entry_id_start,
                      const std::string &raw_entry_id_end) const {
  auto entry_id_start = Stream::EntryID::toEntryID(raw_entry_id_start, 0);
  auto entry_id_end = Stream::EntryID::toEntryID(raw_entry_id_end, std::numeric_limits<int32_t>::max());
  assert(entry_id_start < entry_id_end);
  assert(entry_id_start.has_value());
  assert(entry_id_end.has_value());
  auto begin_it = get_it_start_at(entry_id_start.value());
  auto end_it = get_it_start_after(entry_id_end.value());
  return std::span<const Stream::Entry>(begin_it, end_it);
}

std::vector<Stream::Entry>::iterator
Stream::get_it_start_at(const Stream::EntryID &pv) const {
  Stream::Entry entry_pv(pv);
  return std::lower_bound(entries->begin(), entries->end(), entry_pv,
                          [](const Stream::Entry &a, const Stream::Entry &b) {
                            return a.id < b.id;
                          });
}

std::vector<Stream::Entry>::iterator
Stream::get_it_start_after(const Stream::EntryID &pv) const {
  Stream::Entry entry_pv(pv);
  return std::upper_bound(entries->begin(), entries->end(), entry_pv,
                          [](const Stream::Entry &a, const Stream::Entry &b) {
                            return a.id < b.id;
                          });
}