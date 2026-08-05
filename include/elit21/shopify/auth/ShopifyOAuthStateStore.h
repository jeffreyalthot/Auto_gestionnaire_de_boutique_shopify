#pragma once

#include <chrono>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

namespace elit21::shopify::auth {

struct ShopifyOAuthState {
    std::string value;
    std::string shop;
    std::string redirect_uri;
    std::chrono::steady_clock::time_point expires_at;
};

class ShopifyOAuthStateStore {
public:
    std::string issue(std::chrono::minutes ttl = std::chrono::minutes(10));
    std::string issueForShop(std::string shop,
                             std::string redirect_uri,
                             std::chrono::minutes ttl = std::chrono::minutes(10));
    bool consume(const std::string& state);
    std::optional<ShopifyOAuthState> consumeDetails(const std::string& state);
    void purgeExpired();
    [[nodiscard]] std::size_t size() const;

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, ShopifyOAuthState> states_;
};

} // namespace elit21::shopify::auth
