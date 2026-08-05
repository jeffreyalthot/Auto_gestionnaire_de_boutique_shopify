#include "elit21/util/TimeUtil.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>

namespace elit21::util {
namespace {
std::tm utcTm(std::time_t value) {
    std::tm result{};
#ifdef _WIN32
    gmtime_s(&result, &value);
#else
    gmtime_r(&value, &result);
#endif
    return result;
}

std::tm localTm(std::time_t value) {
    std::tm result{};
#ifdef _WIN32
    localtime_s(&result, &value);
#else
    localtime_r(&value, &result);
#endif
    return result;
}
}

std::string utcNowIso() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    const auto tm = utcTm(time);
    std::ostringstream output;
    output << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return output.str();
}

std::string localNowDisplay() {
    const auto time = std::time(nullptr);
    const auto tm = localTm(time);
    std::ostringstream output;
    output << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return output.str();
}

std::int64_t unixMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

void sleepInterruptible(std::chrono::milliseconds duration, const bool& stop_flag) {
    auto remaining = duration;
    while (remaining.count() > 0 && !stop_flag) {
        const auto slice = std::min(remaining, std::chrono::milliseconds(100));
        std::this_thread::sleep_for(slice);
        remaining -= slice;
    }
}
}
