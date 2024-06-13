#include "Storage.h"
#include <optional>

std::optional<std::string> KVStorage::get(const std::string& k) {
   auto it = data_.find(k);
   if (it == data_.end()) {
    return std::nullopt;
   }
   return it->second;
}

void KVStorage::set(std::string k, std::string v) {
    data_.insert_or_assign(std::move(k), std::move(v));
}
