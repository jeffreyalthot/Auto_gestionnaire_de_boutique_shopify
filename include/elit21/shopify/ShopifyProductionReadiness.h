#pragma once

#include "elit21/config/Config.h"
#include "elit21/json/Json.h"
#include "elit21/storage/Database.h"

#include <string>
#include <vector>

namespace elit21::shopify {

struct ShopifyReadinessCheck {
    std::string id;
    std::string level;
    std::string message;
    bool passed{false};
    int weight{0};
    [[nodiscard]] Json toJson() const;
};

struct ShopifyReadinessReport {
    int score{0};
    bool production_ready{false};
    std::vector<ShopifyReadinessCheck> checks;
    [[nodiscard]] Json toJson() const;
};

class ShopifyProductionReadiness {
public:
    static ShopifyReadinessReport evaluate(const Config& config, const Database* database = nullptr);
};

} // namespace elit21::shopify
