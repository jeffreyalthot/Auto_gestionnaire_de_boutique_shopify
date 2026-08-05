#pragma once
#include <chrono>
#include <string>
namespace elit21::util {
std::string utcNowIso();
std::string localNowDisplay();
std::int64_t unixMillis();
void sleepInterruptible(std::chrono::milliseconds duration, const bool& stop_flag);
}
