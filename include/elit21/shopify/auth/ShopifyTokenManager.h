#pragma once

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
