#pragma once

#include <chrono>

static const long long get_nanoseconds() {
    auto now = std::chrono::system_clock::now();
    auto now_dur = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
    return now_dur.count();
}
