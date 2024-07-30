#pragma once

#include <algorithm>
#include <asio/steady_timer.hpp>
#include <memory>
#include <optional>
#include <span>
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

    static std::optional<EntryID> toEntryID(const std::string &s,
                                            int32_t default_sequence = -1);

    bool operator<=(const EntryID &other) const;

    bool operator<(const EntryID &other) const;

    bool operator==(const EntryID &other) const = default;

    int64_t timestamp{0};
    int32_t sequence{0};

    std::string to_string() const;

    static const EntryID kZeroEntryID;
    static const EntryID kMaxEntryID;
  };

  struct Entry {
    explicit Entry(EntryID id) : id(std::move(id)) {}
    EntryID id;
    std::vector<std::string> inner_kv;
  };

  Stream() : entries(std::make_shared<std::vector<Entry>>()) {}

  std::optional<EntryID> get_valid_entry_id(const Entry &new_entry,
                                            StreamError &err) const;

  std::optional<std::string> add_entry(Entry entry, StreamError &err);

  std::span<const Entry> extract_range(const std::string &start,
                                       const std::string &end) const;

  std::span<const Entry> extract_from_exclusive(const std::string &start) const;

  void add_wating_timers(const std::shared_ptr<asio::steady_timer>& waiting_timer);

  EntryID get_last_entry_id() const;

private:
  template <bool Inclusive = true>
  std::span<const Entry> inner_extract_range(const EntryID &start,
                                             const EntryID &end) const {
    auto begin_it = get_it<Inclusive>(start);
    auto end_it = get_it<false>(end);
    return std::span<const Stream::Entry>(begin_it, end_it);
  }

  template <bool Inclusive>
  std::vector<Entry>::iterator get_it(const EntryID &entry_id) const {
    Stream::Entry entry_pv(entry_id);
    if constexpr (Inclusive) {
      if (entry_id == EntryID::kZeroEntryID) {
        return entries->begin();
      }
      return std::lower_bound(
          entries->begin(), entries->end(), entry_pv,
          [](const Stream::Entry &a, const Stream::Entry &b) {
            return a.id < b.id;
          });
    }
    if (entry_id == EntryID::kMaxEntryID) {
      return entries->end();
    }
    return std::upper_bound(entries->begin(), entries->end(), entry_pv,
                            [](const Stream::Entry &a, const Stream::Entry &b) {
                              return a.id < b.id;
                            });
  }

  std::shared_ptr<std::vector<Entry>> entries;
};