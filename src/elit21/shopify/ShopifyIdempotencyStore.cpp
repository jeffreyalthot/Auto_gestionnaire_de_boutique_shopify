#include "elit21/shopify/ShopifyIdempotencyStore.h"

#include "elit21/shopify/ShopifyIdempotencyKey.h"

namespace elit21::shopify {

Result<ShopifyIdempotencyHandle> ShopifyIdempotencyStore::reserve(
    const std::string& operation_name,
    const Json& variables,
    const std::string& business_key,
    int ttl_hours) {
    if (operation_name.empty() || business_key.empty()) {
        return Result<ShopifyIdempotencyHandle>::failure(
            "Shopify operation name and business key are required");
    }
    const auto key = ShopifyIdempotencyKey::deterministic(
        operation_name, variables, business_key);
    const auto parameter_hash = ShopifyIdempotencyKey::parameterHash(variables);
    auto reservation = database_.reserveShopifyIdempotency(
        key, operation_name, parameter_hash, ttl_hours);
    if (!reservation) {
        return Result<ShopifyIdempotencyHandle>::failure(reservation.error());
    }
    ShopifyIdempotencyHandle output;
    output.key = key;
    output.operation_name = operation_name;
    output.parameter_hash = parameter_hash;
    output.status = reservation.value().status;
    output.cached_response = reservation.value().response_json;
    output.newly_reserved = reservation.value().inserted;
    return Result<ShopifyIdempotencyHandle>::success(std::move(output));
}

Result<void> ShopifyIdempotencyStore::markInProgress(
    const ShopifyIdempotencyHandle& handle) {
    if (handle.key.empty()) return Result<void>::failure("Idempotency handle is empty");
    return database_.markShopifyIdempotencyInProgress(handle.key);
}

Result<void> ShopifyIdempotencyStore::complete(
    const ShopifyIdempotencyHandle& handle,
    const Json& response) {
    if (handle.key.empty()) return Result<void>::failure("Idempotency handle is empty");
    return database_.completeShopifyIdempotency(handle.key, response.dump());
}

Result<void> ShopifyIdempotencyStore::fail(
    const ShopifyIdempotencyHandle& handle,
    const std::string& error) {
    if (handle.key.empty()) return Result<void>::failure("Idempotency handle is empty");
    return database_.failShopifyIdempotency(handle.key, error);
}

} // namespace elit21::shopify
