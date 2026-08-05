#pragma once

#include "elit21/core/Result.h"

#include <cstdint>
#include <string>

namespace elit21::shopify {

class ShopifyTimestamp {
public:
    static Result<std::int64_t> toUnixSeconds(const std::string& value);
    static Result<std::string> normalizeUtc(const std::string& value);
    static Result<int> compare(const std::string& left, const std::string& right);
};

} // namespace elit21::shopify
