#include "Helpers.h"
#include <chrono>

uint64_t get_current_timestamp_ms() {
    auto now = std::chrono::system_clock::now();

    // Convert the time_point to a duration since epoch
    auto duration = now.time_since_epoch();

    // Convert the duration to milliseconds
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}