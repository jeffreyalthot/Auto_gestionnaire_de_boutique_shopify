#include "elit21/shopify/auth/ShopifyTokenManager.h"

#include "elit21/json/Json.h"
#include "elit21/security/Crypto.h"
#include "elit21/security/DataEncryptor.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <system_error>

namespace elit21::shopify::auth {
namespace {
constexpr std::uintmax_t maximum_token_file_bytes = 1024U * 1024U;
}

void ShopifyTokenManager::store(ShopifyAccessToken token) {
    std::lock_guard lock(mutex_);
    token_ = std::move(token);
}

void ShopifyTokenManager::clear() {
    std::lock_guard lock(mutex_);
    if (token_) {
        std::fill(token_->access_token.begin(), token_->access_token.end(), '\0');
        std::fill(token_->scope.begin(), token_->scope.end(), '\0');
    }
    token_.reset();
}

std::optional<ShopifyAccessToken> ShopifyTokenManager::get() const {
    std::lock_guard lock(mutex_);
    return token_;
}

bool ShopifyTokenManager::ready() const {
    std::lock_guard lock(mutex_);
    return token_ && token_->valid();
}

bool ShopifyTokenManager::hasScopes(const ShopifyScopes& required) const {
    std::lock_guard lock(mutex_);
    return token_ && token_->scopes().containsAll(required);
}

ShopifyScopes ShopifyTokenManager::missingScopes(const ShopifyScopes& required) const {
    std::lock_guard lock(mutex_);
    return token_ ? required.missingFrom(token_->scopes()) : required;
}

std::string ShopifyTokenManager::fingerprint() const {
    std::lock_guard lock(mutex_);
    if (!token_ || token_->access_token.empty()) return {};
    return crypto::sha256Hex(token_->access_token).substr(0, 16);
}

Json ShopifyTokenManager::sanitizedMetadata() const {
    std::lock_guard lock(mutex_);
    Json output = Json::object();
    output.set("ready", token_ && token_->valid());
    output.set("associated_user_scope", token_ && token_->associated_user_scope);
    output.set("associated_user_id", token_ ? token_->associated_user_id : std::string{});
    output.set("scope_count", token_ ? static_cast<int>(token_->scopes().values().size()) : 0);
    output.set("fingerprint", token_ && !token_->access_token.empty()
        ? crypto::sha256Hex(token_->access_token).substr(0, 16) : std::string{});
    return output;
}

Result<void> ShopifyTokenManager::saveEncrypted(const std::string& path,
                                                const std::string& master_key,
                                                const std::string& associated_data) const {
    std::optional<ShopifyAccessToken> snapshot;
    {
        std::lock_guard lock(mutex_);
        snapshot = token_;
    }
    if (!snapshot || !snapshot->valid()) return Result<void>::failure("No Shopify token to save");
    if (master_key.size() < 16) return Result<void>::failure("Shopify token master key is too short");
    Json document = Json::object();
    document.set("format", "elit21-shopify-token-v2");
    document.set("access_token", snapshot->access_token);
    document.set("scope", snapshot->scope);
    document.set("associated_user_id", snapshot->associated_user_id);
    document.set("associated_user_scope", snapshot->associated_user_scope);
    auto encrypted = security::DataEncryptor::encrypt(document.dump(), master_key, associated_data);
    if (!encrypted) return Result<void>::failure(encrypted.error());

    const std::filesystem::path target(path);
    if (const auto parent = target.parent_path(); !parent.empty()) std::filesystem::create_directories(parent);
    const auto temporary = target.string() + ".tmp-" + crypto::randomHex(6);
    {
        std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
        if (!output) return Result<void>::failure("Cannot write encrypted Shopify token temporary file");
        output.write(encrypted.value().data(), static_cast<std::streamsize>(encrypted.value().size()));
        output.flush();
        if (!output) { std::error_code error; std::filesystem::remove(temporary, error);
            return Result<void>::failure("Cannot flush encrypted Shopify token file"); }
    }
    std::error_code error;
    std::filesystem::rename(temporary, target, error);
    if (error) {
        std::filesystem::remove(target, error); error.clear();
        std::filesystem::rename(temporary, target, error);
    }
    if (error) { std::filesystem::remove(temporary, error);
        return Result<void>::failure("Cannot atomically replace encrypted Shopify token file"); }
    std::filesystem::permissions(target,
        std::filesystem::perms::owner_read | std::filesystem::perms::owner_write,
        std::filesystem::perm_options::replace, error);
    return Result<void>::success();
}

Result<void> ShopifyTokenManager::loadEncrypted(const std::string& path,
                                                const std::string& master_key,
                                                const std::string& associated_data) {
    std::error_code file_error;
    const auto size = std::filesystem::file_size(path, file_error);
    if (file_error) return Result<void>::failure("Cannot inspect encrypted Shopify token file");
    if (size == 0 || size > maximum_token_file_bytes)
        return Result<void>::failure("Encrypted Shopify token file has an invalid size");
    std::ifstream input(path, std::ios::binary);
    if (!input) return Result<void>::failure("Cannot read encrypted Shopify token file");
    std::ostringstream buffer; buffer << input.rdbuf();
    auto decrypted = security::DataEncryptor::decrypt(buffer.str(), master_key, associated_data);
    if (!decrypted) return Result<void>::failure(decrypted.error());
    auto parsed = Json::parse(decrypted.value());
    if (!parsed || !parsed.value().isObject()) return Result<void>::failure("Decrypted Shopify token document is invalid");
    const auto format = parsed.value().getString("format");
    if (!format.empty() && format != "elit21-shopify-token-v2")
        return Result<void>::failure("Unsupported encrypted Shopify token format");
    ShopifyAccessToken token;
    token.access_token = parsed.value().getString("access_token");
    token.scope = parsed.value().getString("scope");
    token.associated_user_id = parsed.value().getString("associated_user_id");
    token.associated_user_scope = parsed.value().getBool("associated_user_scope", !token.associated_user_id.empty());
    if (!token.valid()) return Result<void>::failure("Encrypted Shopify token is invalid");
    store(std::move(token));
    return Result<void>::success();
}

} // namespace elit21::shopify::auth
