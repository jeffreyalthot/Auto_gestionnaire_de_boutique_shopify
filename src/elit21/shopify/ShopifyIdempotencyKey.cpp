#include "elit21/shopify/ShopifyIdempotencyKey.h"

#include "elit21/security/Crypto.h"

#include <algorithm>
#include <cctype>

namespace elit21::shopify {
namespace {

std::string uuidFromHex(std::string value) {
    if (value.size() < 32) value.append(32 - value.size(), '0');
    value.resize(32);
    return value.substr(0, 8) + "-" + value.substr(8, 4) + "-" +
           value.substr(12, 4) + "-" + value.substr(16, 4) + "-" +
           value.substr(20, 12);
}

} // namespace

std::string ShopifyIdempotencyKey::random() {
    return uuidFromHex(crypto::randomHex(16));
}

std::string ShopifyIdempotencyKey::deterministic(const std::string& operation_name,
                                                  const Json& variables,
                                                  const std::string& business_key) {
    return uuidFromHex(crypto::sha256Hex(operation_name + "\n" + business_key + "\n" +
                                         variables.dump()));
}

Result<void> ShopifyIdempotencyKey::validate(const std::string& key) {
    if (key.size() < 16 || key.size() > 255) {
        return Result<void>::failure("Shopify idempotency key length is outside 16..255");
    }
    if (!std::all_of(key.begin(), key.end(), [](unsigned char character) {
            return std::isalnum(character) != 0 || character == '-' || character == '_' ||
                   character == ':' || character == '.';
        })) {
        return Result<void>::failure("Shopify idempotency key contains unsupported characters");
    }
    return Result<void>::success();
}

std::string ShopifyIdempotencyKey::parameterHash(const Json& variables) {
    return crypto::sha256Hex(variables.dump());
}

} // namespace elit21::shopify
