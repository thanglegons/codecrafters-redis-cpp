#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

struct Stream {

  enum StreamError {
    OK,
    entryIDParsedError,
    entryIDIsZero,
    entryIDIsEqualOrSmaller
  };

  struct EntryID {

    EntryID(int64_t timestamp, int32_t sequence)
        : timestamp(timestamp), sequence(sequence) {}

    static std::optional<EntryID> toEntryID(const std::string &s);

    bool operator<=(const EntryID &other) const;

    bool operator==(const EntryID &other) const = default;

    int64_t timestamp{0};
    int32_t sequence{0};

    std::string to_string() const;

    static const EntryID kZeroEntryID;
  };

  struct Entry {
    explicit Entry(EntryID id) : id(std::move(id)) {}
    EntryID id;
    std::unordered_map<std::string, std::string> inner_kv;
  };

  Stream() : entries(std::make_shared<std::vector<Entry>>()) {}

  std::optional<EntryID> get_valid_entry_id(const Entry &new_entry,
                                            StreamError &err) const;

  std::optional<std::string> add_entry(Entry entry, StreamError &err);

private:
  std::shared_ptr<std::vector<Entry>> entries;
};