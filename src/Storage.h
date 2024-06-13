#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <optional>

class KVStorage {
public:
    void set(std::string k, std::string v, uint32_t expiring_time_ms = -1);

    std::optional<std::string> get(const std::string& k);

private:
    struct ExpiringValue {
        std::string value;
        uint64_t expired_ts_ms;
    };
    std::unordered_map<std::string, ExpiringValue> data_;
};