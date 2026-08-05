#pragma once

#include "elit21/json/Json.h"

#include <string>
#include <vector>

namespace elit21::shopify::graphql {

struct ShopifyUserError {
    std::vector<std::string> field;
    std::string message;
    std::string code;

    [[nodiscard]] std::string fieldPath() const;
    [[nodiscard]] std::string describe() const;
};

class ShopifyUserErrorParser {
public:
    static std::vector<ShopifyUserError> parse(const Json& payload);
    static std::vector<ShopifyUserError> findRecursively(const Json& payload, std::size_t maximum_depth = 8);
    static std::string join(const std::vector<ShopifyUserError>& errors);
    static bool containsCode(const std::vector<ShopifyUserError>& errors, const std::string& code);
};

} // namespace elit21::shopify::graphql
