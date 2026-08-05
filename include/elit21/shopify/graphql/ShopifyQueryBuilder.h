#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"

#include <string>
#include <vector>

namespace elit21::shopify::graphql {

class ShopifyQueryBuilder {
public:
    static std::string gid(const std::string& resource, const std::string& id);
    static Result<void> validateGid(const std::string& value, const std::string& expected_resource = {});
    static Result<void> validateSearchField(const std::string& field);
    static Result<void> validateIso8601(const std::string& value);
    static Json pageVariables(int first, const std::string& after = {});
    static std::string escapeSearch(const std::string& value);
    static std::string quoteSearch(const std::string& value);
    static std::string joinSearch(const std::vector<std::string>& predicates, const std::string& logical = "AND");
    static Result<std::string> safeFieldEquals(const std::string& field, const std::string& value);
    static std::string fieldEquals(const std::string& field, const std::string& value);
    static Result<std::string> safeUpdatedAfter(const std::string& iso8601);
    static std::string updatedAfter(const std::string& iso8601);
};

} // namespace elit21::shopify::graphql
