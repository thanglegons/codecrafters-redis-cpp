#include "Stream.hpp"
#include "Helpers.h"
#include <iostream>

const Stream::EntryID Stream::EntryID::kZeroEntryID{0, 0};

std::optional<Stream::EntryID>
Stream::EntryID::toEntryID(const std::string &s) {
  if (s == "*") {
    return std::make_optional<EntryID>(-1, -1);
  }
  auto parts = splitString(s, '-');
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
  if (*this == other) {
    return true;
  }
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