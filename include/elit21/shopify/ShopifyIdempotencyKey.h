#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"

#include <string>

namespace elit21::shopify {

class ShopifyIdempotencyKey {
public:
    static std::string random();
    static std::string deterministic(const std::string& operation_name,
                                     const Json& variables,
                                     const std::string& business_key);
    static Result<void> validate(const std::string& key);
    static std::string parameterHash(const Json& variables);
};

} // namespace elit21::shopify
