#pragma once

#include "elit21/config/Config.h"
#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/shopify/ShopifyClient.h"

#include <string>
#include <utility>
#include <vector>

namespace elit21 {

struct ShopifyWebhookRegistryAction {
    enum class Type { Keep, Create, DeleteDuplicate, DeleteObsolete, IgnoreExternal };
    Type type{Type::Keep};
    std::string subscription_id;
    std::string topic;
    std::string callback_url;
    std::string reason;
    [[nodiscard]] Json toJson() const;
};

struct ShopifyWebhookRegistryPlan {
    int required{0};
    int exact_existing{0};
    int missing{0};
    int duplicates{0};
    int obsolete_managed{0};
    int external_ignored{0};
    std::vector<ShopifyWebhookRegistryAction> actions;
    [[nodiscard]] Json toJson() const;
};

struct ShopifyWebhookRegistrySummary {
    int required{0};
    int existing{0};
    int created{0};
    int planned{0};
    int duplicates{0};
    int obsolete{0};
    int deleted{0};
    int ignored_external{0};
    int failed{0};
    std::vector<std::string> messages;
    [[nodiscard]] Json toJson() const;
};

class ShopifyWebhookRegistry {
public:
    ShopifyWebhookRegistry(ShopifyClient& client, ShopifyConfig config)
        : client_(client), config_(std::move(config)) {}

    Result<ShopifyWebhookRegistrySummary> reconcile(bool dry_run, bool prune_managed = false);
    [[nodiscard]] ShopifyWebhookRegistryPlan plan(
        const std::vector<ShopifyWebhookSubscriptionRef>& current) const;
    [[nodiscard]] static std::vector<std::pair<std::string, std::string>> requiredTopics();

private:
    [[nodiscard]] std::string callbackUrl(const std::string& path) const;
    [[nodiscard]] bool managedCallback(const std::string& callback_url) const;

    ShopifyClient& client_;
    ShopifyConfig config_;
};

} // namespace elit21
