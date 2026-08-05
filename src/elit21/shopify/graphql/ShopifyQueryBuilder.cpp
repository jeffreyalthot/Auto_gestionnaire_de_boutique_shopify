#include "elit21/shopify/graphql/ShopifyQueryBuilder.h"

#include "elit21/util/StringUtil.h"

#include <algorithm>
#include <regex>
#include <sstream>

namespace elit21::shopify::graphql {

std::string ShopifyQueryBuilder::gid(const std::string& resource, const std::string& id) {
    if (id.rfind("gid://shopify/", 0) == 0) return id;
    if (!std::regex_match(resource, std::regex(R"(^[A-Za-z][A-Za-z0-9]*$)")) || id.empty()) return {};
    return "gid://shopify/" + resource + '/' + id;
}

Result<void> ShopifyQueryBuilder::validateGid(const std::string& value,
                                               const std::string& expected_resource) {
    static const std::regex pattern(R"(^gid://shopify/([A-Za-z][A-Za-z0-9]*)/([^/\s?#]+)$)");
    std::smatch match;
    if (!std::regex_match(value, match, pattern)) return Result<void>::failure("Invalid Shopify GID");
    if (!expected_resource.empty() && match[1].str() != expected_resource)
        return Result<void>::failure("Shopify GID resource mismatch: expected " + expected_resource);
    return Result<void>::success();
}

Result<void> ShopifyQueryBuilder::validateSearchField(const std::string& field) {
    static const std::regex pattern(R"(^[a-z][a-z0-9_]{0,63}$)");
    return std::regex_match(field, pattern) ? Result<void>::success()
        : Result<void>::failure("Invalid Shopify search field");
}

Result<void> ShopifyQueryBuilder::validateIso8601(const std::string& value) {
    static const std::regex pattern(
        R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(?:\.\d{1,9})?(?:Z|[+-]\d{2}:\d{2})$)");
    return std::regex_match(value, pattern) ? Result<void>::success()
        : Result<void>::failure("Invalid ISO-8601 timestamp for Shopify search");
}

Json ShopifyQueryBuilder::pageVariables(int first, const std::string& after) {
    Json variables = Json::object();
    variables.set("first", std::clamp(first, 1, 250));
    if (!after.empty()) variables.set("after", after);
    return variables;
}

std::string ShopifyQueryBuilder::escapeSearch(const std::string& value) {
    std::string output; output.reserve(value.size() * 2);
    for (const unsigned char ch : value) {
        if (ch < 0x20 || ch == 0x7f) continue;
        if (ch == '\\' || ch == '"' || ch == ':' || ch == '(' || ch == ')') output.push_back('\\');
        output.push_back(static_cast<char>(ch));
    }
    return output;
}

std::string ShopifyQueryBuilder::quoteSearch(const std::string& value) {
    return '"' + escapeSearch(value) + '"';
}

std::string ShopifyQueryBuilder::joinSearch(const std::vector<std::string>& predicates,
                                             const std::string& logical) {
    const auto keyword = util::upper(logical) == "OR" ? " OR " : " AND ";
    std::ostringstream output; bool wrote = false;
    for (const auto& predicate : predicates) {
        if (util::trim(predicate).empty()) continue;
        if (wrote) output << keyword;
        output << '(' << predicate << ')'; wrote = true;
    }
    return output.str();
}

Result<std::string> ShopifyQueryBuilder::safeFieldEquals(const std::string& field, const std::string& value) {
    auto valid = validateSearchField(field);
    return valid ? Result<std::string>::success(field + ':' + quoteSearch(value))
                 : Result<std::string>::failure(valid.error());
}

std::string ShopifyQueryBuilder::fieldEquals(const std::string& field, const std::string& value) {
    auto result = safeFieldEquals(field, value); return result ? result.value() : std::string{};
}

Result<std::string> ShopifyQueryBuilder::safeUpdatedAfter(const std::string& iso8601) {
    auto valid = validateIso8601(iso8601);
    return valid ? Result<std::string>::success("updated_at:>=" + quoteSearch(iso8601))
                 : Result<std::string>::failure(valid.error());
}

std::string ShopifyQueryBuilder::updatedAfter(const std::string& iso8601) {
    auto result = safeUpdatedAfter(iso8601); return result ? result.value() : std::string{};
}

} // namespace elit21::shopify::graphql
