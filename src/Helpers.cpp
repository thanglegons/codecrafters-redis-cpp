#include "Helpers.h"
#include <chrono>
#include <random>
#include <sstream>

uint64_t get_current_timestamp_ms() {
    auto now = std::chrono::system_clock::now();

    // Convert the time_point to a duration since epoch
    auto duration = now.time_since_epoch();

    // Convert the duration to milliseconds
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

std::string random_string(int n) {
    const std::string chars = "abcdefghijklmnopqrstuvwxyz0123456789";
    std::mt19937 rng(static_cast<unsigned long>(std::time(nullptr))); // Seed with current time
    std::uniform_int_distribution<> dist(0, chars.size() - 1);

    std::string result;
    result.reserve(n);

    for (size_t i = 0; i < n; ++i) {
        result += chars[dist(rng)];
    }

    return result;
}

std::vector<std::string> splitString(const std::string &s, char delim) {
    std::istringstream iss(s);
    std::vector<std::string> results;
    std::string temp;
    while (std::getline(iss, temp, delim)) {
        results.emplace_back(temp);
    }
    if (!s.empty() && s.back() == delim) {
        results.emplace_back();
    }
    return results;
}