#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/storage/Database.h"

#include <string>

namespace elit21::shopify {

struct ShopifyIdempotencyHandle {
    std::string key;
    std::string operation_name;
    std::string parameter_hash;
    std::string status;
    std::string cached_response;
    bool newly_reserved{false};

    [[nodiscard]] bool completed() const noexcept { return status == "completed"; }
    [[nodiscard]] bool concurrent() const noexcept {
        return !newly_reserved && status == "in_progress";
    }
};

class ShopifyIdempotencyStore {
public:
    explicit ShopifyIdempotencyStore(Database& database) : database_(database) {}

    Result<ShopifyIdempotencyHandle> reserve(const std::string& operation_name,
                                             const Json& variables,
                                             const std::string& business_key,
                                             int ttl_hours = 24);
    Result<void> markInProgress(const ShopifyIdempotencyHandle& handle);
    Result<void> complete(const ShopifyIdempotencyHandle& handle,
                          const Json& response = Json::object());
    Result<void> fail(const ShopifyIdempotencyHandle& handle,
                      const std::string& error);

private:
    Database& database_;
};

} // namespace elit21::shopify
