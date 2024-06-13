#pragma once

#include <unordered_map>
#include <string>
#include <optional>

class KVStorage {
public:

    void set(std::string k, std::string v);

    std::optional<std::string> get(const std::string& k);

private:
    std::unordered_map<std::string, std::string> data_;
};