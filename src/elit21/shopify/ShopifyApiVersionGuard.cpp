#include "elit21/shopify/ShopifyApiVersionGuard.h"

#include "elit21/shopify/ShopifyApiVersion.h"
#include "elit21/util/TimeUtil.h"

#include <array>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <map>
#include <sstream>

namespace elit21::shopify {
namespace {

struct VersionWindow {
    const char* version;
    const char* release;
    const char* support_end;
};

constexpr std::array<VersionWindow, 7> windows{{
    {"2025-07", "2025-07-01", "2026-07-16"},
    {"2025-10", "2025-10-01", "2026-10-16"},
    {"2026-01", "2026-01-01", "2027-01-16"},
    {"2026-04", "2026-04-01", "2027-04-16"},
    {"2026-07", "2026-07-01", "2027-07-16"},
    {"2026-10", "2026-10-01", "2027-10-16"},
    {"2027-01", "2027-01-01", "2028-01-16"},
}};

std::string todayUtc() {
    const auto value = util::utcNowIso();
    return value.size() >= 10 ? value.substr(0, 10) : value;
}

Result<std::time_t> parseDate(const std::string& value) {
    std::tm time{};
    std::istringstream input(value);
    input >> std::get_time(&time, "%Y-%m-%d");
    if (input.fail()) return Result<std::time_t>::failure("Invalid ISO date: " + value);
#ifdef _WIN32
    return Result<std::time_t>::success(_mkgmtime(&time));
#else
    return Result<std::time_t>::success(timegm(&time));
#endif
}

const VersionWindow* windowFor(const std::string& version) {
    for (const auto& window : windows) {
        if (version == window.version) return &window;
    }
    return nullptr;
}

} // namespace

bool ShopifyApiVersionAssessment::usable() const noexcept {
    return version_matches &&
           (state == ShopifyApiVersionState::supported ||
            state == ShopifyApiVersionState::expiring_soon);
}

Json ShopifyApiVersionAssessment::toJson() const {
    Json output = Json::object();
    output.set("requested", requested);
    output.set("served", served);
    output.set("release_date", release_date);
    output.set("support_ends_at", support_ends_at);
    output.set("state", ShopifyApiVersionGuard::stateName(state));
    output.set("days_remaining", days_remaining);
    output.set("version_matches", version_matches);
    output.set("usable", usable());
    return output;
}

std::string ShopifyApiVersionGuard::currentStable() {
    return "2026-07";
}

Result<ShopifyApiVersionAssessment> ShopifyApiVersionGuard::assess(
    const std::string& requested,
    const std::string& served,
    const std::string& today) {
    ShopifyApiVersionAssessment output;
    output.requested = requested;
    output.served = served.empty() ? requested : served;
    output.version_matches = output.requested == output.served;

    if (requested == "unstable") {
        output.state = ShopifyApiVersionState::unstable;
        return Result<ShopifyApiVersionAssessment>::success(std::move(output));
    }
    auto parsed = ShopifyApiVersion::parse(requested);
    if (!parsed) {
        output.state = ShopifyApiVersionState::invalid;
        return Result<ShopifyApiVersionAssessment>::failure(parsed.error());
    }
    const auto* window = windowFor(requested);
    if (window == nullptr) {
        output.state = ShopifyApiVersionState::invalid;
        return Result<ShopifyApiVersionAssessment>::failure(
            "Shopify API version is outside the version guard schedule: " + requested);
    }
    output.release_date = window->release;
    output.support_ends_at = window->support_end;

    if (!output.version_matches) {
        output.state = ShopifyApiVersionState::fall_forward;
        return Result<ShopifyApiVersionAssessment>::success(std::move(output));
    }

    const auto current = parseDate(today.empty() ? todayUtc() : today);
    const auto end = parseDate(output.support_ends_at);
    const auto release = parseDate(output.release_date);
    if (!current || !end || !release) {
        return Result<ShopifyApiVersionAssessment>::failure("Unable to parse API lifecycle dates");
    }
    output.days_remaining = static_cast<int>(
        std::chrono::duration_cast<std::chrono::hours>(
            std::chrono::system_clock::from_time_t(end.value()) -
            std::chrono::system_clock::from_time_t(current.value())).count() / 24);

    if (current.value() < release.value()) {
        output.state = ShopifyApiVersionState::release_candidate;
    } else if (output.days_remaining < 0) {
        output.state = ShopifyApiVersionState::expired;
    } else if (output.days_remaining <= 90) {
        output.state = ShopifyApiVersionState::expiring_soon;
    } else {
        output.state = ShopifyApiVersionState::supported;
    }
    return Result<ShopifyApiVersionAssessment>::success(std::move(output));
}

Result<void> ShopifyApiVersionGuard::requireExactServedVersion(const std::string& requested,
                                                               const std::string& served) {
    if (served.empty()) return Result<void>::success();
    if (requested != served) {
        return Result<void>::failure("Shopify served API version " + served +
                                     " instead of requested version " + requested +
                                     "; refusing silent fall-forward");
    }
    return Result<void>::success();
}

std::vector<std::string> ShopifyApiVersionGuard::knownStableVersions() {
    std::vector<std::string> output;
    output.reserve(windows.size());
    for (const auto& window : windows) output.emplace_back(window.version);
    return output;
}

std::string ShopifyApiVersionGuard::stateName(ShopifyApiVersionState state) {
    switch (state) {
        case ShopifyApiVersionState::supported: return "supported";
        case ShopifyApiVersionState::expiring_soon: return "expiring_soon";
        case ShopifyApiVersionState::expired: return "expired";
        case ShopifyApiVersionState::release_candidate: return "release_candidate";
        case ShopifyApiVersionState::unstable: return "unstable";
        case ShopifyApiVersionState::fall_forward: return "fall_forward";
        case ShopifyApiVersionState::invalid: return "invalid";
    }
    return "invalid";
}

} // namespace elit21::shopify
