#include "elit21/shopify/ShopifyScopes.h"

#include "elit21/util/StringUtil.h"

#include <algorithm>
#include <regex>
#include <sstream>

namespace elit21::shopify {

ShopifyScopes::ShopifyScopes(std::vector<std::string> scopes) {
    for (auto& scope : scopes) add(std::move(scope));
}

bool ShopifyScopes::validScope(const std::string& scope) {
    static const std::regex pattern(R"(^[a-z][a-z0-9_]{2,80}$)");
    return std::regex_match(scope, pattern);
}

void ShopifyScopes::add(std::string scope) {
    scope = util::lower(util::trim(std::move(scope)));
    if (validScope(scope)) scopes_.insert(std::move(scope));
}

void ShopifyScopes::remove(const std::string& scope) {
    scopes_.erase(util::lower(util::trim(scope)));
}

bool ShopifyScopes::contains(const std::string& scope) const {
    return scopes_.contains(util::lower(util::trim(scope)));
}

bool ShopifyScopes::containsAll(const ShopifyScopes& required) const {
    return std::all_of(required.scopes_.begin(), required.scopes_.end(),
                       [&](const auto& scope) { return scopes_.contains(scope); });
}

ShopifyScopes ShopifyScopes::missingFrom(const ShopifyScopes& granted) const {
    ShopifyScopes missing;
    for (const auto& scope : scopes_) if (!granted.contains(scope)) missing.add(scope);
    return missing;
}

ShopifyScopes ShopifyScopes::intersection(const ShopifyScopes& other) const {
    ShopifyScopes output;
    for (const auto& scope : scopes_) if (other.contains(scope)) output.add(scope);
    return output;
}

std::string ShopifyScopes::commaSeparated() const {
    std::ostringstream output;
    bool first = true;
    for (const auto& scope : scopes_) {
        if (!first) output << ',';
        first = false;
        output << scope;
    }
    return output.str();
}

std::vector<std::string> ShopifyScopes::sorted() const {
    return {scopes_.begin(), scopes_.end()};
}

ShopifyScopes ShopifyScopes::parse(const std::string& value) {
    ShopifyScopes result;
    std::size_t start = 0;
    while (start <= value.size()) {
        const auto comma = value.find(',', start);
        result.add(value.substr(start, comma == std::string::npos ? std::string::npos : comma - start));
        if (comma == std::string::npos) break;
        start = comma + 1;
    }
    return result;
}

ShopifyScopes ShopifyScopes::autonomousManagerDefaults() {
    return ShopifyScopes({
        "read_products", "write_products", "read_inventory", "write_inventory",
        "read_orders", "write_orders", "read_customers", "write_customers",
        "read_fulfillments", "write_fulfillments",
        "read_merchant_managed_fulfillment_orders",
        "write_merchant_managed_fulfillment_orders",
        "read_locations", "read_publications", "write_publications",
        "read_files", "write_files", "read_discounts", "write_discounts"
    });
}

ShopifyScopes ShopifyScopes::privacyMandatory() {
    return ShopifyScopes({"read_customers", "read_orders"});
}

} // namespace elit21::shopify
