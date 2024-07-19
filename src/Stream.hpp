#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <string>
#include <vector>

struct Stream {

  enum StreamError { OK, entryIDParsedError, entryIDIsZero, entryIDIsEqualOrSmaller };

  struct EntryID {

    EntryID(uint64_t timestamp, uint32_t sequence)
        : timestamp(timestamp), sequence(sequence) {}

    static std::optional<EntryID> toEntryID(const std::string &s);

    bool operator<=(const EntryID &other) const;

    bool operator==(const EntryID &other) const = default;

    uint64_t timestamp{0};
    uint32_t sequence{0};

    static const EntryID kZeroEntryID;
  };

  struct Entry {
    explicit Entry(EntryID id) : id(std::move(id)) {}
    EntryID id;
    std::unordered_map<std::string, std::string> inner_kv;
  };

  Stream() : entries(std::make_shared<std::vector<Entry>>()) {}

  StreamError is_valid_new_entry(const Entry &new_entry) const;

  StreamError add_entry(Entry entry);

private:
  std::shared_ptr<std::vector<Entry>> entries;
};