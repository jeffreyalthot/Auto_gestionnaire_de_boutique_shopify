#pragma once

#include "elit21/config/Config.h"
#include "elit21/core/Result.h"
#include "elit21/json/Json.h"

#include <string>
#include <vector>

namespace elit21 {
class Database;
class ShopifyClient;
}

namespace elit21::shopify {

enum class ShopifyGovernanceSeverity { info, warning, critical };

struct ShopifyGovernanceCheck {
    std::string name;
    bool passed{false};
    ShopifyGovernanceSeverity severity{ShopifyGovernanceSeverity::warning};
    std::string detail;

    [[nodiscard]] Json toJson() const;
};

struct ShopifyGovernanceReport {
    int score{0};
    bool production_ready{false};
    std::string evaluated_at;
    std::vector<ShopifyGovernanceCheck> checks;

    [[nodiscard]] Json toJson() const;
};

class ShopifyOperationalGovernance {
public:
    static ShopifyGovernanceReport evaluateOffline(const Config& config, Database* database = nullptr);
    static Result<ShopifyGovernanceReport> evaluateLive(const Config& config,
                                                        ShopifyClient& client,
                                                        Database& database);
    static std::string severityName(ShopifyGovernanceSeverity severity);
};

} // namespace elit21::shopify
