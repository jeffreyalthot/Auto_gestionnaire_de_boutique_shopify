#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"

#include <string>
#include <vector>

namespace elit21::shopify {

enum class ShopifyApiVersionState {
    supported,
    expiring_soon,
    expired,
    release_candidate,
    unstable,
    invalid,
    fall_forward
};

struct ShopifyApiVersionAssessment {
    std::string requested;
    std::string served;
    std::string release_date;
    std::string support_ends_at;
    ShopifyApiVersionState state{ShopifyApiVersionState::invalid};
    int days_remaining{-1};
    bool version_matches{false};

    [[nodiscard]] bool usable() const noexcept;
    [[nodiscard]] Json toJson() const;
};

class ShopifyApiVersionGuard {
public:
    static std::string currentStable();
    static Result<ShopifyApiVersionAssessment> assess(const std::string& requested,
                                                       const std::string& served = {},
                                                       const std::string& today = {});
    static Result<void> requireExactServedVersion(const std::string& requested,
                                                  const std::string& served);
    static std::vector<std::string> knownStableVersions();
    static std::string stateName(ShopifyApiVersionState state);
};

} // namespace elit21::shopify
