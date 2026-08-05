#include "elit21/shopify/ShopifyMutationCoordinator.h"

#include "elit21/shopify/ShopifyClient.h"
#include "elit21/shopify/graphql/ShopifyUserErrorParser.h"
#include "elit21/storage/Database.h"

namespace elit21::shopify {

Json ShopifyMutationReceipt::toJson() const {
    Json output = Json::object();
    output.set("operation_name", operation_name);
    output.set("business_key", business_key);
    output.set("idempotency_key", idempotency_key);
    output.set("replayed", replayed);
    output.set("response", response);
    return output;
}

ShopifyMutationCoordinator::ShopifyMutationCoordinator(ShopifyClient& client, Database& database)
    : client_(client), database_(database), idempotency_(database) {}

Result<ShopifyMutationReceipt> ShopifyMutationCoordinator::replay(
    const ShopifyIdempotencyHandle& handle,
    const std::string& business_key) const {
    auto parsed = Json::parse(handle.cached_response.empty() ? "{}" : handle.cached_response);
    if (!parsed) return Result<ShopifyMutationReceipt>::failure(parsed.error());
    ShopifyMutationReceipt receipt;
    receipt.operation_name = handle.operation_name;
    receipt.business_key = business_key;
    receipt.idempotency_key = handle.key;
    receipt.response = parsed.value();
    receipt.replayed = true;
    return Result<ShopifyMutationReceipt>::success(std::move(receipt));
}

Result<ShopifyMutationReceipt> ShopifyMutationCoordinator::execute(
    const std::string& operation_name,
    const std::string& query,
    Json variables,
    const std::string& business_key,
    int maximum_attempts,
    bool inject_idempotency_key) {
    if (operation_name.empty() || query.empty() || business_key.empty()) {
        return Result<ShopifyMutationReceipt>::failure(
            "Shopify mutation operation name, query and business key are required");
    }
    if (query.find("mutation") == std::string::npos) {
        return Result<ShopifyMutationReceipt>::failure(
            "ShopifyMutationCoordinator only accepts GraphQL mutations");
    }

    auto handle = idempotency_.reserve(operation_name, variables, business_key, 24);
    if (!handle) return Result<ShopifyMutationReceipt>::failure(handle.error());
    if (handle.value().completed()) return replay(handle.value(), business_key);
    if (handle.value().concurrent()) {
        return Result<ShopifyMutationReceipt>::failure(
            "Shopify mutation is already in progress for business key: " + business_key);
    }
    if (inject_idempotency_key) variables.set("idempotencyKey", handle.value().key);

    if (auto started = idempotency_.markInProgress(handle.value()); !started) {
        return Result<ShopifyMutationReceipt>::failure(started.error());
    }
    auto response = client_.graphql(query, variables, maximum_attempts);
    if (!response) {
        idempotency_.fail(handle.value(), response.error());
        database_.audit("error", "shopify_mutation", operation_name + " failed",
                        Json::object().dump(), handle.value().key);
        return Result<ShopifyMutationReceipt>::failure(response.error());
    }

    const auto user_errors = graphql::ShopifyUserErrorParser::findRecursively(response.value());
    if (!user_errors.empty()) {
        const auto message = graphql::ShopifyUserErrorParser::join(user_errors);
        idempotency_.fail(handle.value(), message);
        Json context = Json::object();
        context.set("operation_name", operation_name);
        context.set("business_key", business_key);
        context.set("idempotency_key", handle.value().key);
        context.set("error", message);
        database_.audit("warning", "shopify_mutation", "Shopify user error", context.dump(), handle.value().key);
        return Result<ShopifyMutationReceipt>::failure(message);
    }

    if (auto completed = idempotency_.complete(handle.value(), response.value()); !completed) {
        return Result<ShopifyMutationReceipt>::failure(completed.error());
    }
    Json context = Json::object();
    context.set("operation_name", operation_name);
    context.set("business_key", business_key);
    context.set("idempotency_key", handle.value().key);
    database_.audit("info", "shopify_mutation", operation_name + " completed",
                    context.dump(), handle.value().key);

    ShopifyMutationReceipt receipt;
    receipt.operation_name = operation_name;
    receipt.business_key = business_key;
    receipt.idempotency_key = handle.value().key;
    receipt.response = response.value();
    return Result<ShopifyMutationReceipt>::success(std::move(receipt));
}

} // namespace elit21::shopify
