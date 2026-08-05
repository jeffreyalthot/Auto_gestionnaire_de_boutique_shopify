from pathlib import Path
root=Path('/mnt/data/ELIT21_Shopify_AliExpress_Autonomous_Manager-v6.0.0-SHOPIFY-TRANSACTIONAL')

def write(rel,text):
 p=root/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_text(text,encoding='utf-8')

def replace(rel,old,new,count=-1):
 p=root/rel;s=p.read_text(encoding='utf-8')
 if old not in s: raise SystemExit(f'missing {rel}: {old[:100]!r}')
 p.write_text(s.replace(old,new,count),encoding='utf-8')

write('include/elit21/shopify/auth/ShopifyTokenManager.h',r'''#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/shopify/auth/ShopifyOAuthClient.h"

#include <mutex>
#include <optional>
#include <string>

namespace elit21::shopify::auth {

class ShopifyTokenManager {
public:
    void store(ShopifyAccessToken token);
    void clear();
    [[nodiscard]] std::optional<ShopifyAccessToken> get() const;
    [[nodiscard]] bool ready() const;
    [[nodiscard]] bool hasScopes(const ShopifyScopes& required) const;
    [[nodiscard]] ShopifyScopes missingScopes(const ShopifyScopes& required) const;
    [[nodiscard]] std::string fingerprint() const;
    [[nodiscard]] Json sanitizedMetadata() const;

    Result<void> saveEncrypted(const std::string& path,
                               const std::string& master_key,
                               const std::string& associated_data = "shopify-token") const;
    Result<void> loadEncrypted(const std::string& path,
                               const std::string& master_key,
                               const std::string& associated_data = "shopify-token");

private:
    mutable std::mutex mutex_;
    std::optional<ShopifyAccessToken> token_;
};

} // namespace elit21::shopify::auth
''')

write('src/elit21/shopify/auth/ShopifyTokenManager.cpp',r'''#include "elit21/shopify/auth/ShopifyTokenManager.h"

#include "elit21/json/Json.h"
#include "elit21/security/Crypto.h"
#include "elit21/security/DataEncryptor.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <system_error>

namespace elit21::shopify::auth {
namespace {
constexpr std::uintmax_t maximum_token_file_bytes = 1024U * 1024U;
}

void ShopifyTokenManager::store(ShopifyAccessToken token) {
    std::lock_guard lock(mutex_);
    token_ = std::move(token);
}

void ShopifyTokenManager::clear() {
    std::lock_guard lock(mutex_);
    if (token_) {
        std::fill(token_->access_token.begin(), token_->access_token.end(), '\0');
        std::fill(token_->scope.begin(), token_->scope.end(), '\0');
    }
    token_.reset();
}

std::optional<ShopifyAccessToken> ShopifyTokenManager::get() const {
    std::lock_guard lock(mutex_);
    return token_;
}

bool ShopifyTokenManager::ready() const {
    std::lock_guard lock(mutex_);
    return token_ && token_->valid();
}

bool ShopifyTokenManager::hasScopes(const ShopifyScopes& required) const {
    std::lock_guard lock(mutex_);
    return token_ && token_->scopes().containsAll(required);
}

ShopifyScopes ShopifyTokenManager::missingScopes(const ShopifyScopes& required) const {
    std::lock_guard lock(mutex_);
    return token_ ? required.missingFrom(token_->scopes()) : required;
}

std::string ShopifyTokenManager::fingerprint() const {
    std::lock_guard lock(mutex_);
    if (!token_ || token_->access_token.empty()) return {};
    return crypto::sha256Hex(token_->access_token).substr(0, 16);
}

Json ShopifyTokenManager::sanitizedMetadata() const {
    std::lock_guard lock(mutex_);
    Json output = Json::object();
    output.set("ready", token_ && token_->valid());
    output.set("associated_user_scope", token_ && token_->associated_user_scope);
    output.set("associated_user_id", token_ ? token_->associated_user_id : std::string{});
    output.set("scope_count", token_ ? static_cast<int>(token_->scopes().values().size()) : 0);
    output.set("fingerprint", token_ && !token_->access_token.empty()
        ? crypto::sha256Hex(token_->access_token).substr(0, 16) : std::string{});
    return output;
}

Result<void> ShopifyTokenManager::saveEncrypted(const std::string& path,
                                                const std::string& master_key,
                                                const std::string& associated_data) const {
    std::optional<ShopifyAccessToken> snapshot;
    {
        std::lock_guard lock(mutex_);
        snapshot = token_;
    }
    if (!snapshot || !snapshot->valid()) return Result<void>::failure("No Shopify token to save");
    if (master_key.size() < 16) return Result<void>::failure("Shopify token master key is too short");
    Json document = Json::object();
    document.set("format", "elit21-shopify-token-v2");
    document.set("access_token", snapshot->access_token);
    document.set("scope", snapshot->scope);
    document.set("associated_user_id", snapshot->associated_user_id);
    document.set("associated_user_scope", snapshot->associated_user_scope);
    auto encrypted = security::DataEncryptor::encrypt(document.dump(), master_key, associated_data);
    if (!encrypted) return Result<void>::failure(encrypted.error());

    const std::filesystem::path target(path);
    if (const auto parent = target.parent_path(); !parent.empty()) std::filesystem::create_directories(parent);
    const auto temporary = target.string() + ".tmp-" + crypto::randomHex(6);
    {
        std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
        if (!output) return Result<void>::failure("Cannot write encrypted Shopify token temporary file");
        output.write(encrypted.value().data(), static_cast<std::streamsize>(encrypted.value().size()));
        output.flush();
        if (!output) { std::error_code error; std::filesystem::remove(temporary, error);
            return Result<void>::failure("Cannot flush encrypted Shopify token file"); }
    }
    std::error_code error;
    std::filesystem::rename(temporary, target, error);
    if (error) {
        std::filesystem::remove(target, error); error.clear();
        std::filesystem::rename(temporary, target, error);
    }
    if (error) { std::filesystem::remove(temporary, error);
        return Result<void>::failure("Cannot atomically replace encrypted Shopify token file"); }
    std::filesystem::permissions(target,
        std::filesystem::perms::owner_read | std::filesystem::perms::owner_write,
        std::filesystem::perm_options::replace, error);
    return Result<void>::success();
}

Result<void> ShopifyTokenManager::loadEncrypted(const std::string& path,
                                                const std::string& master_key,
                                                const std::string& associated_data) {
    std::error_code file_error;
    const auto size = std::filesystem::file_size(path, file_error);
    if (file_error) return Result<void>::failure("Cannot inspect encrypted Shopify token file");
    if (size == 0 || size > maximum_token_file_bytes)
        return Result<void>::failure("Encrypted Shopify token file has an invalid size");
    std::ifstream input(path, std::ios::binary);
    if (!input) return Result<void>::failure("Cannot read encrypted Shopify token file");
    std::ostringstream buffer; buffer << input.rdbuf();
    auto decrypted = security::DataEncryptor::decrypt(buffer.str(), master_key, associated_data);
    if (!decrypted) return Result<void>::failure(decrypted.error());
    auto parsed = Json::parse(decrypted.value());
    if (!parsed || !parsed.value().isObject()) return Result<void>::failure("Decrypted Shopify token document is invalid");
    const auto format = parsed.value().getString("format");
    if (!format.empty() && format != "elit21-shopify-token-v2")
        return Result<void>::failure("Unsupported encrypted Shopify token format");
    ShopifyAccessToken token;
    token.access_token = parsed.value().getString("access_token");
    token.scope = parsed.value().getString("scope");
    token.associated_user_id = parsed.value().getString("associated_user_id");
    token.associated_user_scope = parsed.value().getBool("associated_user_scope", !token.associated_user_id.empty());
    if (!token.valid()) return Result<void>::failure("Encrypted Shopify token is invalid");
    store(std::move(token));
    return Result<void>::success();
}

} // namespace elit21::shopify::auth
''')

write('include/elit21/shopify/auth/ShopifyTokenRefreshPolicy.h',r'''#pragma once

#include "elit21/shopify/ShopifyScopes.h"
#include "elit21/shopify/auth/ShopifyOAuthClient.h"

#include <string>

namespace elit21::shopify::auth {

enum class ShopifyTokenAction { none, retry, verify_scopes, reauthorize };

class ShopifyTokenRefreshPolicy {
public:
    static bool shouldReauthorize(bool token_present, bool unauthorized_response) {
        return !token_present || unauthorized_response;
    }
    static bool shouldReauthorize(const ShopifyAccessToken* token,
                                  long http_status,
                                  const ShopifyScopes& required_scopes);
    static bool isAuthenticationFailure(long http_status, const std::string& response_body);
    static ShopifyTokenAction action(const ShopifyAccessToken* token,
                                     long http_status,
                                     const std::string& response_body,
                                     const ShopifyScopes& required_scopes);
    static std::string actionName(ShopifyTokenAction action);
};

} // namespace elit21::shopify::auth
''')

write('src/elit21/shopify/auth/ShopifyTokenRefreshPolicy.cpp',r'''#include "elit21/shopify/auth/ShopifyTokenRefreshPolicy.h"

#include "elit21/util/StringUtil.h"

namespace elit21::shopify::auth {

bool ShopifyTokenRefreshPolicy::isAuthenticationFailure(long http_status,
                                                         const std::string& response_body) {
    if (http_status == 401) return true;
    const auto lower = util::lower(response_body);
    return lower.find("invalid api key") != std::string::npos ||
           lower.find("invalid access token") != std::string::npos ||
           lower.find("token expired") != std::string::npos ||
           lower.find("session expired") != std::string::npos;
}

ShopifyTokenAction ShopifyTokenRefreshPolicy::action(const ShopifyAccessToken* token,
                                                      long http_status,
                                                      const std::string& response_body,
                                                      const ShopifyScopes& required_scopes) {
    if (!token || !token->valid()) return ShopifyTokenAction::reauthorize;
    if (isAuthenticationFailure(http_status, response_body)) return ShopifyTokenAction::reauthorize;
    if (!token->scopes().containsAll(required_scopes)) return ShopifyTokenAction::verify_scopes;
    if (http_status == 403 || util::lower(response_body).find("access denied") != std::string::npos)
        return ShopifyTokenAction::verify_scopes;
    if (http_status == 429 || http_status >= 500) return ShopifyTokenAction::retry;
    return ShopifyTokenAction::none;
}

bool ShopifyTokenRefreshPolicy::shouldReauthorize(const ShopifyAccessToken* token,
                                                   long http_status,
                                                   const ShopifyScopes& required_scopes) {
    return action(token, http_status, {}, required_scopes) == ShopifyTokenAction::reauthorize;
}

std::string ShopifyTokenRefreshPolicy::actionName(ShopifyTokenAction value) {
    switch (value) {
        case ShopifyTokenAction::retry: return "retry";
        case ShopifyTokenAction::verify_scopes: return "verify_scopes";
        case ShopifyTokenAction::reauthorize: return "reauthorize";
        default: return "none";
    }
}

} // namespace elit21::shopify::auth
''')

write('include/elit21/shopify/graphql/GraphqlError.h',r'''#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"

#include <chrono>
#include <string>
#include <vector>

namespace elit21::shopify::graphql {

struct GraphqlError {
    std::string message;
    std::vector<std::string> path;
    std::string code;
    Json extensions{Json::object()};

    static Result<GraphqlError> fromJson(const Json& value);
    [[nodiscard]] bool throttled() const;
    [[nodiscard]] bool retryable() const;
    [[nodiscard]] std::chrono::milliseconds retryDelay(std::chrono::milliseconds fallback =
        std::chrono::milliseconds(1000)) const;
    [[nodiscard]] std::string pathString() const;
    [[nodiscard]] std::string describe() const;
};

} // namespace elit21::shopify::graphql
''')

write('src/elit21/shopify/graphql/GraphqlError.cpp',r'''#include "elit21/shopify/graphql/GraphqlError.h"

#include "elit21/util/StringUtil.h"

#include <algorithm>
#include <sstream>

namespace elit21::shopify::graphql {

Result<GraphqlError> GraphqlError::fromJson(const Json& value) {
    if (!value.isObject()) return Result<GraphqlError>::failure("GraphQL error must be an object");
    GraphqlError error;
    error.message = value.getString("message");
    if (error.message.empty()) return Result<GraphqlError>::failure("GraphQL error message is missing");
    const auto path_value = value.get("path");
    if (path_value.isArray()) {
        for (std::size_t index = 0; index < path_value.size(); ++index)
            error.path.push_back(path_value.at(index).scalarStringValue());
    }
    error.extensions = value.get("extensions");
    if (!error.extensions.isObject()) error.extensions = Json::object();
    error.code = error.extensions.getString("code", value.getString("code"));
    return Result<GraphqlError>::success(std::move(error));
}

bool GraphqlError::throttled() const {
    const auto normalized_code = util::lower(code);
    const auto normalized_message = util::lower(message);
    return normalized_code == "throttled" || normalized_code == "throttle_exceeded" ||
           normalized_message.find("throttl") != std::string::npos;
}

bool GraphqlError::retryable() const {
    const auto normalized_code = util::lower(code);
    return throttled() || normalized_code == "internal_server_error" ||
           normalized_code == "service_unavailable" || normalized_code == "timeout" ||
           normalized_code == "temporarily_unavailable";
}

std::chrono::milliseconds GraphqlError::retryDelay(std::chrono::milliseconds fallback) const {
    const double seconds = extensions.getNumber("retryAfter",
        extensions.getNumber("retry_after", fallback.count() / 1000.0));
    const auto bounded = std::clamp(seconds, 0.05, 300.0);
    return std::chrono::milliseconds(static_cast<long long>(bounded * 1000.0));
}

std::string GraphqlError::pathString() const {
    std::ostringstream output;
    for (std::size_t index = 0; index < path.size(); ++index) {
        if (index) output << '.';
        output << path[index];
    }
    return output.str();
}

std::string GraphqlError::describe() const {
    std::ostringstream output;
    if (!path.empty()) output << pathString() << ": ";
    output << message;
    if (!code.empty()) output << " [" << code << ']';
    return output.str();
}

} // namespace elit21::shopify::graphql
''')

write('include/elit21/shopify/graphql/ShopifyQueryBuilder.h',r'''#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"

#include <string>
#include <vector>

namespace elit21::shopify::graphql {

class ShopifyQueryBuilder {
public:
    static std::string gid(const std::string& resource, const std::string& id);
    static Result<void> validateGid(const std::string& value, const std::string& expected_resource = {});
    static Result<void> validateSearchField(const std::string& field);
    static Result<void> validateIso8601(const std::string& value);
    static Json pageVariables(int first, const std::string& after = {});
    static std::string escapeSearch(const std::string& value);
    static std::string quoteSearch(const std::string& value);
    static std::string joinSearch(const std::vector<std::string>& predicates, const std::string& logical = "AND");
    static Result<std::string> safeFieldEquals(const std::string& field, const std::string& value);
    static std::string fieldEquals(const std::string& field, const std::string& value);
    static Result<std::string> safeUpdatedAfter(const std::string& iso8601);
    static std::string updatedAfter(const std::string& iso8601);
};

} // namespace elit21::shopify::graphql
''')

write('src/elit21/shopify/graphql/ShopifyQueryBuilder.cpp',r'''#include "elit21/shopify/graphql/ShopifyQueryBuilder.h"

#include "elit21/util/StringUtil.h"

#include <algorithm>
#include <regex>
#include <sstream>

namespace elit21::shopify::graphql {

std::string ShopifyQueryBuilder::gid(const std::string& resource, const std::string& id) {
    if (id.rfind("gid://shopify/", 0) == 0) return id;
    if (!std::regex_match(resource, std::regex(R"(^[A-Za-z][A-Za-z0-9]*$)")) || id.empty()) return {};
    return "gid://shopify/" + resource + '/' + id;
}

Result<void> ShopifyQueryBuilder::validateGid(const std::string& value,
                                               const std::string& expected_resource) {
    static const std::regex pattern(R"(^gid://shopify/([A-Za-z][A-Za-z0-9]*)/([^/\s?#]+)$)");
    std::smatch match;
    if (!std::regex_match(value, match, pattern)) return Result<void>::failure("Invalid Shopify GID");
    if (!expected_resource.empty() && match[1].str() != expected_resource)
        return Result<void>::failure("Shopify GID resource mismatch: expected " + expected_resource);
    return Result<void>::success();
}

Result<void> ShopifyQueryBuilder::validateSearchField(const std::string& field) {
    static const std::regex pattern(R"(^[a-z][a-z0-9_]{0,63}$)");
    return std::regex_match(field, pattern) ? Result<void>::success()
        : Result<void>::failure("Invalid Shopify search field");
}

Result<void> ShopifyQueryBuilder::validateIso8601(const std::string& value) {
    static const std::regex pattern(
        R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(?:\.\d{1,9})?(?:Z|[+-]\d{2}:\d{2})$)");
    return std::regex_match(value, pattern) ? Result<void>::success()
        : Result<void>::failure("Invalid ISO-8601 timestamp for Shopify search");
}

Json ShopifyQueryBuilder::pageVariables(int first, const std::string& after) {
    Json variables = Json::object();
    variables.set("first", std::clamp(first, 1, 250));
    if (!after.empty()) variables.set("after", after);
    return variables;
}

std::string ShopifyQueryBuilder::escapeSearch(const std::string& value) {
    std::string output; output.reserve(value.size() * 2);
    for (const unsigned char ch : value) {
        if (ch < 0x20 || ch == 0x7f) continue;
        if (ch == '\\' || ch == '"' || ch == ':' || ch == '(' || ch == ')') output.push_back('\\');
        output.push_back(static_cast<char>(ch));
    }
    return output;
}

std::string ShopifyQueryBuilder::quoteSearch(const std::string& value) {
    return '"' + escapeSearch(value) + '"';
}

std::string ShopifyQueryBuilder::joinSearch(const std::vector<std::string>& predicates,
                                             const std::string& logical) {
    const auto keyword = util::upper(logical) == "OR" ? " OR " : " AND ";
    std::ostringstream output; bool wrote = false;
    for (const auto& predicate : predicates) {
        if (util::trim(predicate).empty()) continue;
        if (wrote) output << keyword;
        output << '(' << predicate << ')'; wrote = true;
    }
    return output.str();
}

Result<std::string> ShopifyQueryBuilder::safeFieldEquals(const std::string& field, const std::string& value) {
    auto valid = validateSearchField(field);
    return valid ? Result<std::string>::success(field + ':' + quoteSearch(value))
                 : Result<std::string>::failure(valid.error());
}

std::string ShopifyQueryBuilder::fieldEquals(const std::string& field, const std::string& value) {
    auto result = safeFieldEquals(field, value); return result ? result.value() : std::string{};
}

Result<std::string> ShopifyQueryBuilder::safeUpdatedAfter(const std::string& iso8601) {
    auto valid = validateIso8601(iso8601);
    return valid ? Result<std::string>::success("updated_at:>=" + quoteSearch(iso8601))
                 : Result<std::string>::failure(valid.error());
}

std::string ShopifyQueryBuilder::updatedAfter(const std::string& iso8601) {
    auto result = safeUpdatedAfter(iso8601); return result ? result.value() : std::string{};
}

} // namespace elit21::shopify::graphql
''')

# Deduplicate recursively discovered user errors.
p=root/'src/elit21/shopify/graphql/ShopifyUserErrorParser.cpp'
s=p.read_text()
s=s.replace('#include <sstream>','#include <set>\n#include <sstream>')
old='''    std::vector<ShopifyUserError> output;
    collect(payload, 0, maximum_depth, output);
    return output;
'''
new='''    std::vector<ShopifyUserError> collected;
    collect(payload, 0, maximum_depth, collected);
    std::vector<ShopifyUserError> output;
    std::set<std::string> identities;
    for (auto& error : collected) {
        const auto identity = util::lower(error.fieldPath() + "|" + error.code + "|" + error.message);
        if (identities.insert(identity).second) output.push_back(std::move(error));
    }
    return output;
'''
if old not in s: raise SystemExit('user error pattern')
p.write_text(s.replace(old,new),encoding='utf-8')

# Consistent mandatory privacy task names and readable implementation.
write('src/elit21/shopify/webhooks/ShopifyWebhookTopic.cpp',r'''#include "elit21/shopify/webhooks/ShopifyWebhookTopic.h"
#include "elit21/util/StringUtil.h"

#include <unordered_map>

namespace elit21::shopify::webhooks {

ShopifyWebhookTopic parseTopic(const std::string& value) noexcept {
    static const std::unordered_map<std::string, ShopifyWebhookTopic> mapping{
        {"orders/paid", ShopifyWebhookTopic::orders_paid},
        {"orders/cancelled", ShopifyWebhookTopic::orders_cancelled},
        {"refunds/create", ShopifyWebhookTopic::refunds_create},
        {"products/update", ShopifyWebhookTopic::products_update},
        {"products/delete", ShopifyWebhookTopic::products_delete},
        {"inventory_levels/update", ShopifyWebhookTopic::inventory_levels_update},
        {"fulfillments/create", ShopifyWebhookTopic::fulfillments_create},
        {"fulfillments/update", ShopifyWebhookTopic::fulfillments_update},
        {"app/uninstalled", ShopifyWebhookTopic::app_uninstalled},
        {"customers/data_request", ShopifyWebhookTopic::customers_data_request},
        {"customers/redact", ShopifyWebhookTopic::customers_redact},
        {"shop/redact", ShopifyWebhookTopic::shop_redact}
    };
    const auto found = mapping.find(util::lower(util::trim(value)));
    return found == mapping.end() ? ShopifyWebhookTopic::unknown : found->second;
}

std::string topicName(ShopifyWebhookTopic topic) {
    switch (topic) {
        case ShopifyWebhookTopic::orders_paid: return "orders/paid";
        case ShopifyWebhookTopic::orders_cancelled: return "orders/cancelled";
        case ShopifyWebhookTopic::refunds_create: return "refunds/create";
        case ShopifyWebhookTopic::products_update: return "products/update";
        case ShopifyWebhookTopic::products_delete: return "products/delete";
        case ShopifyWebhookTopic::inventory_levels_update: return "inventory_levels/update";
        case ShopifyWebhookTopic::fulfillments_create: return "fulfillments/create";
        case ShopifyWebhookTopic::fulfillments_update: return "fulfillments/update";
        case ShopifyWebhookTopic::app_uninstalled: return "app/uninstalled";
        case ShopifyWebhookTopic::customers_data_request: return "customers/data_request";
        case ShopifyWebhookTopic::customers_redact: return "customers/redact";
        case ShopifyWebhookTopic::shop_redact: return "shop/redact";
        default: return "unknown";
    }
}

std::string taskKind(ShopifyWebhookTopic topic) {
    switch (topic) {
        case ShopifyWebhookTopic::orders_paid: return "shopify_order_paid";
        case ShopifyWebhookTopic::orders_cancelled: return "shopify_order_cancelled";
        case ShopifyWebhookTopic::refunds_create: return "shopify_refund_created";
        case ShopifyWebhookTopic::products_update: return "shopify_product_updated";
        case ShopifyWebhookTopic::products_delete: return "shopify_product_deleted";
        case ShopifyWebhookTopic::inventory_levels_update: return "shopify_inventory_updated";
        case ShopifyWebhookTopic::fulfillments_create:
        case ShopifyWebhookTopic::fulfillments_update: return "shopify_fulfillment_updated";
        case ShopifyWebhookTopic::app_uninstalled: return "shopify_app_uninstalled";
        case ShopifyWebhookTopic::customers_data_request: return "shopify_customers_data_request";
        case ShopifyWebhookTopic::customers_redact: return "shopify_customers_redact";
        case ShopifyWebhookTopic::shop_redact: return "shopify_shop_redact";
        default: return "shopify_webhook_unknown";
    }
}

bool isPrivacyTopic(ShopifyWebhookTopic topic) noexcept {
    return topic == ShopifyWebhookTopic::customers_data_request ||
           topic == ShopifyWebhookTopic::customers_redact || topic == ShopifyWebhookTopic::shop_redact;
}

bool isOrderTopic(ShopifyWebhookTopic topic) noexcept {
    return topic == ShopifyWebhookTopic::orders_paid || topic == ShopifyWebhookTopic::orders_cancelled;
}

std::vector<ShopifyWebhookTopic> requiredTopics() {
    return {ShopifyWebhookTopic::orders_paid, ShopifyWebhookTopic::orders_cancelled,
            ShopifyWebhookTopic::refunds_create, ShopifyWebhookTopic::products_update,
            ShopifyWebhookTopic::products_delete, ShopifyWebhookTopic::inventory_levels_update,
            ShopifyWebhookTopic::fulfillments_create, ShopifyWebhookTopic::fulfillments_update,
            ShopifyWebhookTopic::app_uninstalled, ShopifyWebhookTopic::customers_data_request,
            ShopifyWebhookTopic::customers_redact, ShopifyWebhookTopic::shop_redact};
}

} // namespace elit21::shopify::webhooks
''')

# Unify processor mapping and add defensive request checks.
replace('src/elit21/shopify/ShopifyWebhookProcessor.cpp',
'''#include "elit21/security/Crypto.h"
#include "elit21/util/StringUtil.h"

#include <unordered_map>
''',
'''#include "elit21/security/Crypto.h"
#include "elit21/shopify/webhooks/ShopifyWebhookTopic.h"
#include "elit21/util/StringUtil.h"
''')
start='''std::string ShopifyWebhookProcessor::taskKindForTopic(const std::string& topic) {'''
end='''bool ShopifyWebhookProcessor::shopDomainAccepted'''
p=root/'src/elit21/shopify/ShopifyWebhookProcessor.cpp';s=p.read_text();a=s.index(start);b=s.index(end,a)
s=s[:a]+'''std::string ShopifyWebhookProcessor::taskKindForTopic(const std::string& topic) {
    return shopify::webhooks::taskKind(shopify::webhooks::parseTopic(topic));
}

'''+s[b:]
p.write_text(s,encoding='utf-8')
replace('src/elit21/shopify/ShopifyWebhookProcessor.cpp',
'''    if (!shopDomainAccepted(request)) {
''',
'''    if (request.body.empty() || request.body.size() > 2U * 1024U * 1024U) {
        if (counters_) ++counters_->rejected_events;
        return {413, "application/json", "{\"error\":\"invalid_body_size\"}"};
    }
    const auto content_type = request.headers.find("content-type");
    if (content_type != request.headers.end() &&
        util::lower(content_type->second).find("application/json") == std::string::npos) {
        if (counters_) ++counters_->rejected_events;
        return {415, "application/json", "{\"error\":\"json_required\"}"};
    }
    if (!shopDomainAccepted(request)) {
''')
replace('src/elit21/shopify/ShopifyWebhookProcessor.cpp',
'''    const std::string topic = topicFor(request);
    const std::string payload_hash = crypto::sha256Hex(request.body);
''',
'''    const std::string topic = topicFor(request);
    const auto parsed_topic = shopify::webhooks::parseTopic(topic);
    const std::string payload_hash = crypto::sha256Hex(request.body);
    const auto version_header = request.headers.find("x-shopify-api-version");
    if (version_header != request.headers.end() && version_header->second != config_.api_version) {
        database_.audit("WARNING", "webhook", "Shopify webhook API version differs from configured version",
                        "{\"configured\":\"" + config_.api_version + "\",\"served\":\"" +
                        util::trim(version_header->second) + "\"}", webhook_id);
    }
''')
# remove unused warning parsed_topic by use task kind.
replace('src/elit21/shopify/ShopifyWebhookProcessor.cpp',
'''    const auto task_kind = taskKindForTopic(topic);
''',
'''    const auto task_kind = shopify::webhooks::taskKind(parsed_topic);
''')

# Add test coverage for security/query helpers.
p=root/'tests/TestScenarios.h';s=p.read_text()
s=s.replace('''    if (!ShopifyQueryBuilder::validateGid("gid://shopify/Product/123", "Product") ||
        ShopifyQueryBuilder::validateGid("gid://shopify/Order/123", "Product").ok()) return false;
''','''    if (!ShopifyQueryBuilder::validateGid("gid://shopify/Product/123", "Product") ||
        ShopifyQueryBuilder::validateGid("gid://shopify/Order/123", "Product").ok() ||
        !ShopifyQueryBuilder::safeFieldEquals("status", "open") ||
        ShopifyQueryBuilder::safeFieldEquals("bad field", "open") ||
        !ShopifyQueryBuilder::safeUpdatedAfter("2026-08-05T12:00:00Z") ||
        ShopifyQueryBuilder::safeUpdatedAfter("yesterday")) return false;
''')
s=s.replace('''        if (!tokens.hasScopes(required) || !tokens.saveEncrypted(token_path, master)) return false;
''','''        if (!tokens.hasScopes(required) || tokens.fingerprint().size() != 16 ||
            !tokens.sanitizedMetadata().getBool("ready") || !tokens.saveEncrypted(token_path, master)) return false;
''')
p.write_text(s,encoding='utf-8')

print('phase5 applied')
