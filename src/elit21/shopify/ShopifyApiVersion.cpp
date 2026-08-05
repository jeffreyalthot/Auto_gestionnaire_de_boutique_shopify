#include "elit21/shopify/ShopifyApiVersion.h"

#include "elit21/util/StringUtil.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <regex>
#include <tuple>

namespace elit21::shopify {
namespace {
constexpr std::array<int, 4> release_months{1, 4, 7, 10};
std::string formatVersion(int year, int month) {
    char buffer[16]{};
    std::snprintf(buffer, sizeof(buffer), "%04d-%02d", year, month);
    return buffer;
}
}

ShopifyApiVersion::ShopifyApiVersion(std::string value)
    : value_(util::trim(std::move(value))) {}

int ShopifyApiVersion::year() const noexcept {
    if (value_.size() < 4) return 0;
    try { return std::stoi(value_.substr(0, 4)); } catch (...) { return 0; }
}

int ShopifyApiVersion::month() const noexcept {
    if (value_.size() < 7) return 0;
    try { return std::stoi(value_.substr(5, 2)); } catch (...) { return 0; }
}

int ShopifyApiVersion::quarter() const noexcept {
    const auto found = std::find(release_months.begin(), release_months.end(), month());
    return found == release_months.end() ? 0 : static_cast<int>(found - release_months.begin()) + 1;
}

bool ShopifyApiVersion::isReleaseCandidate() const noexcept {
    return value_.size() > 7 && value_.find("-rc") != std::string::npos;
}

bool ShopifyApiVersion::valid() const noexcept {
    static const std::regex stable(R"(^20\d{2}-(01|04|07|10)$)");
    static const std::regex candidate(R"(^20\d{2}-(01|04|07|10)-rc$)");
    return std::regex_match(value_, stable) || std::regex_match(value_, candidate);
}

std::string ShopifyApiVersion::graphqlPath() const {
    return "/admin/api/" + value_ + "/graphql.json";
}

std::string ShopifyApiVersion::storefrontPath() const {
    return "/api/" + value_ + "/graphql.json";
}

ShopifyApiVersion ShopifyApiVersion::nextQuarter() const {
    int y = year();
    int m = month();
    if (m == 10) { ++y; m = 1; }
    else if (m == 1) m = 4;
    else if (m == 4) m = 7;
    else if (m == 7) m = 10;
    else return ShopifyApiVersion(value_);
    return ShopifyApiVersion(formatVersion(y, m));
}

ShopifyApiVersion ShopifyApiVersion::previousQuarter() const {
    int y = year();
    int m = month();
    if (m == 1) { --y; m = 10; }
    else if (m == 4) m = 1;
    else if (m == 7) m = 4;
    else if (m == 10) m = 7;
    else return ShopifyApiVersion(value_);
    return ShopifyApiVersion(formatVersion(y, m));
}

Result<ShopifyApiVersion> ShopifyApiVersion::parse(const std::string& value) {
    ShopifyApiVersion parsed(value);
    return parsed.valid()
        ? Result<ShopifyApiVersion>::success(parsed)
        : Result<ShopifyApiVersion>::failure("Invalid Shopify API version: " + value);
}

std::vector<ShopifyApiVersion> ShopifyApiVersion::supportedWindow(const ShopifyApiVersion& newest,
                                                                  int releases) {
    std::vector<ShopifyApiVersion> result;
    if (!newest.valid()) return result;
    releases = std::clamp(releases, 1, 20);
    auto current = newest;
    for (int index = 0; index < releases; ++index) {
        result.push_back(current);
        current = current.previousQuarter();
    }
    return result;
}

std::strong_ordering operator<=>(const ShopifyApiVersion& left,
                                 const ShopifyApiVersion& right) noexcept {
    const auto left_key = std::tuple{left.year(), left.month(), left.isReleaseCandidate() ? 0 : 1};
    const auto right_key = std::tuple{right.year(), right.month(), right.isReleaseCandidate() ? 0 : 1};
    return left_key <=> right_key;
}

} // namespace elit21::shopify
