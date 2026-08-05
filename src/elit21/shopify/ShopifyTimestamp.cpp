#include "elit21/shopify/ShopifyTimestamp.h"

#include <cstdio>
#include <ctime>
#include <iomanip>
#include <regex>
#include <sstream>

namespace elit21::shopify {
namespace {

std::time_t utcTime(std::tm* value) {
#ifdef _WIN32
    return static_cast<std::time_t>(_mkgmtime64(value));
#else
    return timegm(value);
#endif
}

} // namespace

Result<std::int64_t> ShopifyTimestamp::toUnixSeconds(const std::string& value) {
    static const std::regex pattern(
        R"(^([0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2})(?:\.[0-9]+)?(Z|[+-][0-9]{2}:[0-9]{2})$)");
    std::smatch match;
    if (!std::regex_match(value, match, pattern)) {
        return Result<std::int64_t>::failure("Invalid Shopify RFC3339 timestamp: " + value);
    }
    std::tm parsed{};
    parsed.tm_year = std::stoi(match[1].str()) - 1900;
    parsed.tm_mon = std::stoi(match[2].str()) - 1;
    parsed.tm_mday = std::stoi(match[3].str());
    parsed.tm_hour = std::stoi(match[4].str());
    parsed.tm_min = std::stoi(match[5].str());
    parsed.tm_sec = std::stoi(match[6].str());
    parsed.tm_isdst = 0;
    if (parsed.tm_mon < 0 || parsed.tm_mon > 11 || parsed.tm_mday < 1 || parsed.tm_mday > 31 ||
        parsed.tm_hour > 23 || parsed.tm_min > 59 || parsed.tm_sec > 60) {
        return Result<std::int64_t>::failure("Shopify timestamp fields are out of range");
    }
    const auto base = utcTime(&parsed);
    if (base == static_cast<std::time_t>(-1)) {
        return Result<std::int64_t>::failure("Shopify timestamp cannot be converted to UTC");
    }
    std::int64_t epoch = static_cast<std::int64_t>(base);
    const auto zone = match[7].str();
    if (zone != "Z") {
        const int sign = zone.front() == '+' ? 1 : -1;
        const int hours = std::stoi(zone.substr(1, 2));
        const int minutes = std::stoi(zone.substr(4, 2));
        if (hours > 23 || minutes > 59) {
            return Result<std::int64_t>::failure("Shopify timestamp timezone is out of range");
        }
        epoch -= sign * static_cast<std::int64_t>(hours * 3600 + minutes * 60);
    }
    return Result<std::int64_t>::success(epoch);
}

Result<std::string> ShopifyTimestamp::normalizeUtc(const std::string& value) {
    auto epoch = toUnixSeconds(value);
    if (!epoch) return Result<std::string>::failure(epoch.error());
    const std::time_t time = static_cast<std::time_t>(epoch.value());
    std::tm utc{};
#ifdef _WIN32
    gmtime_s(&utc, &time);
#else
    gmtime_r(&time, &utc);
#endif
    std::ostringstream output;
    output << std::put_time(&utc, "%Y-%m-%dT%H:%M:%SZ");
    return Result<std::string>::success(output.str());
}

Result<int> ShopifyTimestamp::compare(const std::string& left, const std::string& right) {
    auto lhs = toUnixSeconds(left);
    if (!lhs) return Result<int>::failure(lhs.error());
    auto rhs = toUnixSeconds(right);
    if (!rhs) return Result<int>::failure(rhs.error());
    return Result<int>::success(lhs.value() < rhs.value() ? -1 : (lhs.value() > rhs.value() ? 1 : 0));
}

} // namespace elit21::shopify
