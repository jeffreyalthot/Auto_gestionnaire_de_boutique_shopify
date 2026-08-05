#include "elit21/shopify/auth/ShopifyOAuthCallbackVerifier.h"

#include "elit21/security/Crypto.h"
#include "elit21/shopify/ShopifyEndpoints.h"
#include "elit21/util/StringUtil.h"

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cctype>
#include <limits>
#include <sstream>

namespace elit21::shopify::auth {
namespace {

Result<std::string> percentDecode(const std::string& value) {
    std::string output;
    output.reserve(value.size());
    auto hex = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    };
    for (std::size_t index = 0; index < value.size(); ++index) {
        const char current = value[index];
        if (current == '+') {
            output.push_back(' ');
            continue;
        }
        if (current != '%') {
            output.push_back(current);
            continue;
        }
        if (index + 2 >= value.size()) {
            return Result<std::string>::failure("Malformed percent escape in OAuth callback");
        }
        const int high = hex(value[index + 1]);
        const int low = hex(value[index + 2]);
        if (high < 0 || low < 0) {
            return Result<std::string>::failure("Invalid percent escape in OAuth callback");
        }
        output.push_back(static_cast<char>((high << 4) | low));
        index += 2;
    }
    return Result<std::string>::success(std::move(output));
}

Result<std::int64_t> parseTimestamp(const std::string& value) {
    if (value.empty()) return Result<std::int64_t>::failure("OAuth timestamp is missing");
    std::int64_t timestamp = 0;
    const auto parsed = std::from_chars(value.data(), value.data() + value.size(), timestamp);
    if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size() || timestamp <= 0) {
        return Result<std::int64_t>::failure("OAuth timestamp is invalid");
    }
    return Result<std::int64_t>::success(timestamp);
}

} // namespace

Result<ShopifyOAuthCallback> ShopifyOAuthCallbackVerifier::parse(const std::string& raw_query) {
    if (raw_query.empty() || raw_query.size() > 64U * 1024U) {
        return Result<ShopifyOAuthCallback>::failure("OAuth callback query is empty or too large");
    }
    ShopifyOAuthCallback callback;
    std::size_t cursor = raw_query.front() == '?' ? 1U : 0U;
    while (cursor <= raw_query.size()) {
        const auto ampersand = raw_query.find('&', cursor);
        const auto end = ampersand == std::string::npos ? raw_query.size() : ampersand;
        const auto part = raw_query.substr(cursor, end - cursor);
        if (!part.empty()) {
            const auto equal = part.find('=');
            const auto raw_key = part.substr(0, equal);
            const auto raw_value = equal == std::string::npos ? std::string{} : part.substr(equal + 1);
            auto key = percentDecode(raw_key);
            auto value = percentDecode(raw_value);
            if (!key) return Result<ShopifyOAuthCallback>::failure(key.error());
            if (!value) return Result<ShopifyOAuthCallback>::failure(value.error());
            if (key.value().empty()) {
                return Result<ShopifyOAuthCallback>::failure("OAuth callback contains an empty parameter name");
            }
            if (callback.parameters.contains(key.value())) {
                return Result<ShopifyOAuthCallback>::failure("OAuth callback contains a duplicate parameter: " + key.value());
            }
            callback.parameters.emplace(key.value(), value.value());
        }
        if (ampersand == std::string::npos) break;
        cursor = ampersand + 1;
    }
    callback.shop = callback.parameters["shop"];
    callback.code = callback.parameters["code"];
    callback.state = callback.parameters["state"];
    callback.hmac = util::lower(callback.parameters["hmac"]);
    auto timestamp = parseTimestamp(callback.parameters["timestamp"]);
    if (!timestamp) return Result<ShopifyOAuthCallback>::failure(timestamp.error());
    callback.timestamp = timestamp.value();
    if (!ShopifyEndpoints::isTrustedShopDomain(callback.shop)) {
        return Result<ShopifyOAuthCallback>::failure("OAuth callback shop domain is not trusted");
    }
    if (callback.code.empty() || callback.state.size() < 32 || callback.hmac.size() != 64) {
        return Result<ShopifyOAuthCallback>::failure("OAuth callback is missing code, state or HMAC");
    }
    for (const char character : callback.hmac) {
        if (!std::isxdigit(static_cast<unsigned char>(character))) {
            return Result<ShopifyOAuthCallback>::failure("OAuth callback HMAC is not hexadecimal");
        }
    }
    return Result<ShopifyOAuthCallback>::success(std::move(callback));
}

std::string ShopifyOAuthCallbackVerifier::canonicalMessage(
    const std::map<std::string, std::string>& parameters) {
    std::ostringstream output;
    bool first = true;
    for (const auto& [key, value] : parameters) {
        if (key == "hmac" || key == "signature") continue;
        if (!first) output << '&';
        first = false;
        output << key << '=' << value;
    }
    return output.str();
}

std::string ShopifyOAuthCallbackVerifier::signature(
    const std::map<std::string, std::string>& parameters,
    const std::string& client_secret) {
    return crypto::hmacSha256Hex(client_secret, canonicalMessage(parameters));
}

Result<ShopifyOAuthCallbackVerification> ShopifyOAuthCallbackVerifier::verify(
    const std::string& raw_query,
    const std::string& client_secret,
    std::chrono::seconds maximum_age) {
    if (client_secret.size() < 16) {
        return Result<ShopifyOAuthCallbackVerification>::failure("Shopify client secret is too short");
    }
    maximum_age = std::clamp(maximum_age, std::chrono::seconds(30), std::chrono::seconds(3600));
    auto parsed = parse(raw_query);
    if (!parsed) return Result<ShopifyOAuthCallbackVerification>::failure(parsed.error());
    const auto expected = signature(parsed.value().parameters, client_secret);
    if (!crypto::constantTimeEquals(expected, parsed.value().hmac)) {
        return Result<ShopifyOAuthCallbackVerification>::failure("Shopify OAuth callback HMAC is invalid");
    }
    const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    const auto difference = now >= parsed.value().timestamp
        ? now - parsed.value().timestamp : parsed.value().timestamp - now;
    if (difference > maximum_age.count()) {
        return Result<ShopifyOAuthCallbackVerification>::failure("Shopify OAuth callback timestamp is stale");
    }
    auto state = state_store_.consumeDetails(parsed.value().state);
    if (!state) {
        return Result<ShopifyOAuthCallbackVerification>::failure("Shopify OAuth state is missing, expired or already consumed");
    }
    const auto callback_shop = ShopifyEndpoints::normalizeShop(parsed.value().shop);
    if (!state->shop.empty() && callback_shop != ShopifyEndpoints::normalizeShop(state->shop)) {
        return Result<ShopifyOAuthCallbackVerification>::failure("Shopify OAuth state belongs to another shop");
    }
    ShopifyOAuthCallbackVerification verification;
    verification.callback = std::move(parsed.value());
    verification.issued_state = std::move(*state);
    verification.canonical_message = canonicalMessage(verification.callback.parameters);
    return Result<ShopifyOAuthCallbackVerification>::success(std::move(verification));
}

} // namespace elit21::shopify::auth
