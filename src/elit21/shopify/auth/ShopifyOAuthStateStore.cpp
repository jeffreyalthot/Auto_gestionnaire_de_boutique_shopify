#include "elit21/shopify/auth/ShopifyOAuthStateStore.h"

#include "elit21/security/Crypto.h"
#include "elit21/shopify/ShopifyEndpoints.h"

namespace elit21::shopify::auth {

std::string ShopifyOAuthStateStore::issue(std::chrono::minutes ttl) {
    return issueForShop({}, {}, ttl);
}

std::string ShopifyOAuthStateStore::issueForShop(std::string shop,
                                                 std::string redirect_uri,
                                                 std::chrono::minutes ttl) {
    ttl = std::max(ttl, std::chrono::minutes(1));
    ShopifyOAuthState entry;
    entry.value = crypto::randomHex(32);
    entry.shop = ShopifyEndpoints::normalizeShop(std::move(shop));
    entry.redirect_uri = std::move(redirect_uri);
    entry.expires_at = std::chrono::steady_clock::now() + ttl;
    std::lock_guard lock(mutex_);
    states_[entry.value] = entry;
    return entry.value;
}

std::optional<ShopifyOAuthState> ShopifyOAuthStateStore::consumeDetails(const std::string& state) {
    std::lock_guard lock(mutex_);
    const auto found = states_.find(state);
    if (found == states_.end()) return std::nullopt;
    auto entry = found->second;
    states_.erase(found);
    if (entry.expires_at < std::chrono::steady_clock::now()) return std::nullopt;
    return entry;
}

bool ShopifyOAuthStateStore::consume(const std::string& state) {
    return consumeDetails(state).has_value();
}

void ShopifyOAuthStateStore::purgeExpired() {
    std::lock_guard lock(mutex_);
    const auto now = std::chrono::steady_clock::now();
    for (auto iterator = states_.begin(); iterator != states_.end();) {
        iterator = iterator->second.expires_at < now ? states_.erase(iterator) : std::next(iterator);
    }
}

std::size_t ShopifyOAuthStateStore::size() const {
    std::lock_guard lock(mutex_);
    return states_.size();
}

} // namespace elit21::shopify::auth
