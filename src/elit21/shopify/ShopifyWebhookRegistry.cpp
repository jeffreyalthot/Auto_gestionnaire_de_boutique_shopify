#include "elit21/shopify/ShopifyWebhookRegistry.h"

#include "elit21/util/StringUtil.h"

#include <map>
#include <set>

namespace elit21 {
namespace {

std::string actionTypeName(ShopifyWebhookRegistryAction::Type type) {
    switch (type) {
        case ShopifyWebhookRegistryAction::Type::Keep: return "keep";
        case ShopifyWebhookRegistryAction::Type::Create: return "create";
        case ShopifyWebhookRegistryAction::Type::DeleteDuplicate: return "delete_duplicate";
        case ShopifyWebhookRegistryAction::Type::DeleteObsolete: return "delete_obsolete";
        case ShopifyWebhookRegistryAction::Type::IgnoreExternal: return "ignore_external";
    }
    return "unknown";
}

std::string normalizedBase(std::string base) {
    base = util::trim(base);
    while (!base.empty() && base.back() == '/') base.pop_back();
    return base;
}

} // namespace

Json ShopifyWebhookRegistryAction::toJson() const {
    Json output = Json::object();
    output.set("type", actionTypeName(type));
    output.set("subscription_id", subscription_id);
    output.set("topic", topic);
    output.set("callback_url", callback_url);
    output.set("reason", reason);
    return output;
}

Json ShopifyWebhookRegistryPlan::toJson() const {
    Json output = Json::object();
    output.set("required", required);
    output.set("exact_existing", exact_existing);
    output.set("missing", missing);
    output.set("duplicates", duplicates);
    output.set("obsolete_managed", obsolete_managed);
    output.set("external_ignored", external_ignored);
    Json action_array = Json::array();
    for (const auto& action : actions) action_array.push(action.toJson());
    output.set("actions", action_array);
    return output;
}

Json ShopifyWebhookRegistrySummary::toJson() const {
    Json output = Json::object();
    output.set("required", required);
    output.set("existing", existing);
    output.set("created", created);
    output.set("planned", planned);
    output.set("duplicates", duplicates);
    output.set("obsolete", obsolete);
    output.set("deleted", deleted);
    output.set("ignored_external", ignored_external);
    output.set("failed", failed);
    Json details = Json::array();
    for (const auto& message : messages) {
        Json item = Json::object();
        item.set("message", message);
        details.push(item);
    }
    output.set("messages", details);
    return output;
}

std::vector<std::pair<std::string, std::string>> ShopifyWebhookRegistry::requiredTopics() {
    return {
        {"ORDERS_PAID", "/webhooks/shopify"},
        {"ORDERS_CANCELLED", "/webhooks/shopify"},
        {"REFUNDS_CREATE", "/webhooks/shopify"},
        {"PRODUCTS_UPDATE", "/webhooks/shopify"},
        {"PRODUCTS_DELETE", "/webhooks/shopify"},
        {"INVENTORY_LEVELS_UPDATE", "/webhooks/shopify"},
        {"FULFILLMENTS_CREATE", "/webhooks/shopify"},
        {"FULFILLMENTS_UPDATE", "/webhooks/shopify"},
        {"APP_UNINSTALLED", "/webhooks/shopify"},
        {"CUSTOMERS_DATA_REQUEST", "/webhooks/shopify/privacy"},
        {"CUSTOMERS_REDACT", "/webhooks/shopify/privacy"},
        {"SHOP_REDACT", "/webhooks/shopify/privacy"}
    };
}

std::string ShopifyWebhookRegistry::callbackUrl(const std::string& path) const {
    const auto base = normalizedBase(config_.webhook_base_url);
    return base + (path.empty() || path.front() == '/' ? path : "/" + path);
}

bool ShopifyWebhookRegistry::managedCallback(const std::string& callback_url) const {
    const auto base = normalizedBase(config_.webhook_base_url);
    if (base.empty()) return false;
    if (callback_url == base) return true;
    return callback_url.size() > base.size() &&
           callback_url.rfind(base, 0) == 0 && callback_url[base.size()] == '/';
}

ShopifyWebhookRegistryPlan ShopifyWebhookRegistry::plan(
    const std::vector<ShopifyWebhookSubscriptionRef>& current) const {
    ShopifyWebhookRegistryPlan output;
    const auto required = requiredTopics();
    output.required = static_cast<int>(required.size());

    std::map<std::string, std::pair<std::string, std::string>> required_by_key;
    for (const auto& [topic, path] : required) {
        const auto callback = callbackUrl(path);
        required_by_key.emplace(topic + "|" + callback, std::make_pair(topic, callback));
    }

    std::map<std::string, std::vector<const ShopifyWebhookSubscriptionRef*>> current_by_key;
    for (const auto& subscription : current) {
        if (!subscription.valid()) continue;
        current_by_key[subscription.topic + "|" + subscription.callback_url].push_back(&subscription);
    }

    std::set<std::string> consumed_ids;
    for (const auto& [key, required_entry] : required_by_key) {
        const auto found = current_by_key.find(key);
        if (found == current_by_key.end() || found->second.empty()) {
            ++output.missing;
            output.actions.push_back({ShopifyWebhookRegistryAction::Type::Create, {},
                                      required_entry.first, required_entry.second,
                                      "Required Shopify webhook is missing"});
            continue;
        }

        const auto* kept = found->second.front();
        consumed_ids.insert(kept->id);
        ++output.exact_existing;
        output.actions.push_back({ShopifyWebhookRegistryAction::Type::Keep, kept->id,
                                  kept->topic, kept->callback_url,
                                  "Exact required subscription already exists"});

        for (std::size_t i = 1; i < found->second.size(); ++i) {
            const auto* duplicate = found->second[i];
            consumed_ids.insert(duplicate->id);
            ++output.duplicates;
            output.actions.push_back({ShopifyWebhookRegistryAction::Type::DeleteDuplicate,
                                      duplicate->id, duplicate->topic, duplicate->callback_url,
                                      "Duplicate managed subscription"});
        }
    }

    for (const auto& subscription : current) {
        if (!subscription.valid() || consumed_ids.find(subscription.id) != consumed_ids.end()) continue;
        if (managedCallback(subscription.callback_url)) {
            ++output.obsolete_managed;
            output.actions.push_back({ShopifyWebhookRegistryAction::Type::DeleteObsolete,
                                      subscription.id, subscription.topic, subscription.callback_url,
                                      "Managed subscription is not part of the current required plan"});
        } else {
            ++output.external_ignored;
            output.actions.push_back({ShopifyWebhookRegistryAction::Type::IgnoreExternal,
                                      subscription.id, subscription.topic, subscription.callback_url,
                                      "External subscription is outside ELIT21 ownership and is preserved"});
        }
    }
    return output;
}

Result<ShopifyWebhookRegistrySummary> ShopifyWebhookRegistry::reconcile(bool dry_run, bool prune_managed) {
    if (config_.webhook_base_url.rfind("https://", 0) != 0) {
        return Result<ShopifyWebhookRegistrySummary>::failure(
            "Shopify webhook_base_url must use HTTPS before subscriptions can be reconciled");
    }
    auto current = client_.webhookSubscriptions(250);
    if (!current) return Result<ShopifyWebhookRegistrySummary>::failure(current.error());

    const auto registry_plan = plan(current.value());
    ShopifyWebhookRegistrySummary summary;
    summary.required = registry_plan.required;
    summary.existing = registry_plan.exact_existing;
    summary.duplicates = registry_plan.duplicates;
    summary.obsolete = registry_plan.obsolete_managed;
    summary.ignored_external = registry_plan.external_ignored;

    for (const auto& action : registry_plan.actions) {
        switch (action.type) {
            case ShopifyWebhookRegistryAction::Type::Keep:
                summary.messages.push_back(action.topic + " already registered");
                break;
            case ShopifyWebhookRegistryAction::Type::Create: {
                if (dry_run) {
                    ++summary.planned;
                    summary.messages.push_back(action.topic + " planned for " + action.callback_url);
                    break;
                }
                auto created = client_.registerWebhook(action.topic, action.callback_url);
                if (!created) {
                    ++summary.failed;
                    summary.messages.push_back(action.topic + " failed: " + created.error());
                } else {
                    ++summary.created;
                    summary.messages.push_back(action.topic + " registered at " + action.callback_url);
                }
                break;
            }
            case ShopifyWebhookRegistryAction::Type::DeleteDuplicate:
            case ShopifyWebhookRegistryAction::Type::DeleteObsolete: {
                if (!prune_managed || dry_run) {
                    ++summary.planned;
                    summary.messages.push_back("Managed deletion planned: " + action.topic + " " + action.callback_url);
                    break;
                }
                auto deleted = client_.deleteWebhook(action.subscription_id);
                if (!deleted) {
                    ++summary.failed;
                    summary.messages.push_back("Deletion failed for " + action.subscription_id + ": " + deleted.error());
                } else {
                    ++summary.deleted;
                    summary.messages.push_back("Deleted managed subscription " + action.subscription_id);
                }
                break;
            }
            case ShopifyWebhookRegistryAction::Type::IgnoreExternal:
                summary.messages.push_back("External subscription preserved: " + action.topic + " " + action.callback_url);
                break;
        }
    }
    return Result<ShopifyWebhookRegistrySummary>::success(std::move(summary));
}

} // namespace elit21
