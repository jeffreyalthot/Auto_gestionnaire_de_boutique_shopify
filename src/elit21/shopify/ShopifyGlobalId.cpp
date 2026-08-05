#include "elit21/shopify/ShopifyGlobalId.h"

#include "elit21/util/StringUtil.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <limits>
#include <sstream>

namespace elit21::shopify {
namespace {

bool validToken(const std::string& value) {
    return !value.empty() && std::all_of(value.begin(), value.end(), [](unsigned char character) {
        return std::isalnum(character) != 0 || character == '_' || character == '-';
    });
}

std::map<std::string, std::string> parseParameters(const std::string& query) {
    std::map<std::string, std::string> output;
    std::size_t offset = 0;
    while (offset <= query.size()) {
        const auto end = query.find('&', offset);
        const auto token = query.substr(offset, end == std::string::npos ? std::string::npos : end - offset);
        if (!token.empty()) {
            const auto equals = token.find('=');
            const auto key = util::urlDecode(token.substr(0, equals));
            const auto value = equals == std::string::npos
                ? std::string{} : util::urlDecode(token.substr(equals + 1));
            if (!key.empty()) output[key] = value;
        }
        if (end == std::string::npos) break;
        offset = end + 1;
    }
    return output;
}

} // namespace

Result<ShopifyGlobalId> ShopifyGlobalId::parse(const std::string& value) {
    constexpr std::string_view prefix{"gid://shopify/"};
    if (value.rfind(prefix.data(), 0) != 0) {
        return Result<ShopifyGlobalId>::failure("Shopify GID must start with gid://shopify/");
    }
    const auto body = value.substr(prefix.size());
    const auto slash = body.find('/');
    if (slash == std::string::npos) {
        return Result<ShopifyGlobalId>::failure("Shopify GID resource type or ID is missing");
    }
    ShopifyGlobalId result;
    result.resource_type_ = body.substr(0, slash);
    auto identifier_and_query = body.substr(slash + 1);
    const auto question = identifier_and_query.find('?');
    result.resource_id_ = identifier_and_query.substr(0, question);
    if (question != std::string::npos) {
        result.parameters_ = parseParameters(identifier_and_query.substr(question + 1));
    }
    if (!result.valid()) {
        return Result<ShopifyGlobalId>::failure("Shopify GID contains an invalid resource type or ID");
    }
    return Result<ShopifyGlobalId>::success(std::move(result));
}

Result<ShopifyGlobalId> ShopifyGlobalId::fromNumeric(const std::string& resource_type,
                                                      std::uint64_t numeric_id) {
    if (!validToken(resource_type) || numeric_id == 0) {
        return Result<ShopifyGlobalId>::failure("Invalid Shopify numeric GID input");
    }
    ShopifyGlobalId result;
    result.resource_type_ = resource_type;
    result.resource_id_ = std::to_string(numeric_id);
    return Result<ShopifyGlobalId>::success(std::move(result));
}

bool ShopifyGlobalId::valid() const noexcept {
    return validToken(resource_type_) && validToken(resource_id_);
}

std::optional<std::uint64_t> ShopifyGlobalId::numericId() const noexcept {
    if (resource_id_.empty()) return std::nullopt;
    std::uint64_t output = 0;
    const auto* begin = resource_id_.data();
    const auto* end = begin + resource_id_.size();
    const auto parsed = std::from_chars(begin, end, output);
    if (parsed.ec != std::errc{} || parsed.ptr != end || output == 0) return std::nullopt;
    return output;
}

std::string ShopifyGlobalId::toString() const {
    if (!valid()) return {};
    std::ostringstream output;
    output << "gid://shopify/" << resource_type_ << '/' << resource_id_;
    bool first = true;
    for (const auto& [key, value] : parameters_) {
        output << (first ? '?' : '&') << util::urlEncode(key);
        if (!value.empty()) output << '=' << util::urlEncode(value);
        first = false;
    }
    return output.str();
}

bool ShopifyGlobalId::isType(const std::string& expected) const noexcept {
    return resource_type_ == expected;
}

} // namespace elit21::shopify
