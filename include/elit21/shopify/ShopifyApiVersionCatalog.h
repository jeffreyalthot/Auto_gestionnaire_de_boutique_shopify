#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"

#include <optional>
#include <string>
#include <vector>

namespace elit21 {
class Database;
class ShopifyClient;
}

namespace elit21::shopify {

struct ShopifyPublicApiVersion {
    std::string handle;
    std::string display_name;
    bool supported{false};

    [[nodiscard]] Json toJson() const;
};

struct ShopifyApiVersionCatalogSnapshot {
    std::string shop_domain;
    std::string captured_at;
    std::vector<ShopifyPublicApiVersion> versions;

    [[nodiscard]] bool supports(const std::string& handle) const;
    [[nodiscard]] std::optional<std::string> latestSupported() const;
    [[nodiscard]] std::optional<std::string> nextAfter(const std::string& handle) const;
    [[nodiscard]] Json toJson() const;
};

class ShopifyApiVersionCatalog {
public:
    static Result<ShopifyApiVersionCatalogSnapshot> parse(const Json& graphql_root,
                                                           const std::string& shop_domain = {});
    static Result<ShopifyApiVersionCatalogSnapshot> discover(ShopifyClient& client);
    static Result<void> persist(Database& database,
                                const ShopifyApiVersionCatalogSnapshot& snapshot);
    static Result<std::optional<ShopifyApiVersionCatalogSnapshot>> load(
        Database& database,
        const std::string& shop_domain);
    static Result<void> requireSupported(const ShopifyApiVersionCatalogSnapshot& snapshot,
                                         const std::string& configured_version);
    static std::string query();
};

} // namespace elit21::shopify
