#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"

#include <filesystem>
#include <string>
#include <vector>

namespace elit21 {
class Database;
}

namespace elit21::shopify {

struct ShopifyApiContractFinding {
    std::string severity;
    std::string path;
    std::string rule;
    std::string message;

    [[nodiscard]] Json toJson() const;
};

struct ShopifyApiContractReport {
    int score{100};
    bool passed{true};
    std::size_t files_scanned{0};
    std::size_t errors{0};
    std::size_t warnings{0};
    std::vector<ShopifyApiContractFinding> findings;

    [[nodiscard]] Json toJson() const;
    [[nodiscard]] std::string markdown() const;
};

class ShopifyApiContractAuditor {
public:
    [[nodiscard]] static Result<ShopifyApiContractReport> audit(
        const std::filesystem::path& project_root,
        Database* database = nullptr);
};

} // namespace elit21::shopify
