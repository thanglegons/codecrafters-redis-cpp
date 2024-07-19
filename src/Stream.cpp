#include "Stream.hpp"
#include "Helpers.h"
#include <iostream>

const Stream::EntryID Stream::EntryID::kZeroEntryID{0, 0};

std::optional<Stream::EntryID>
Stream::EntryID::toEntryID(const std::string &s) {
  auto parts = splitString(s, '-');
  if (parts.size() != 2) {
    return std::nullopt;
  }
  try {
    uint64_t timestamp = std::stoll(parts[0]);
    uint32_t sequence = std::stoi(parts[1]);
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

Stream::StreamError Stream::is_valid_new_entry(const Entry &new_entry) const {
  auto &entry_id = new_entry.id;
  if (entry_id == Stream::EntryID::kZeroEntryID) {
    return StreamError::entryIDIsZero;
  }
  if (!entries->empty() && entry_id <= entries->back().id) {
    return StreamError::entryIDIsEqualOrSmaller;
  }
  return StreamError::OK;
}

Stream::StreamError Stream::add_entry(Entry entry) {
  if (auto err = is_valid_new_entry(entry); err != StreamError::OK) {
    return err;
  }
  entries->emplace_back(std::move(entry));
  return StreamError::OK;
}