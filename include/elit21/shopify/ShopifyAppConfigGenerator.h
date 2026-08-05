#pragma once

#include "elit21/core/Result.h"
#include "elit21/shopify/ShopifyScopes.h"

#include <string>
#include <vector>

namespace elit21::shopify {

struct ShopifyAppConfigurationSpec {
    std::string client_id;
    std::string application_url;
    std::vector<std::string> redirect_urls;
    std::string webhook_uri{"/webhooks/shopify"};
    std::string api_version{"2026-07"};
    ShopifyScopes scopes;
    bool embedded{false};
    bool include_operational_webhooks{true};

    [[nodiscard]] Result<void> validate() const;
};

class ShopifyAppConfigGenerator {
public:
    static Result<std::string> generateToml(const ShopifyAppConfigurationSpec& spec);
    static Result<void> writeAtomically(const ShopifyAppConfigurationSpec& spec,
                                        const std::string& output_path);
    static std::vector<std::string> operationalTopics();
    static std::vector<std::string> complianceTopics();
};

} // namespace elit21::shopify
