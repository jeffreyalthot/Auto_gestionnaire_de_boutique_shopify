#include "elit21/shopify/graphql/ShopifyUserErrorParser.h"

#include "elit21/util/StringUtil.h"

#include <set>
#include <sstream>

namespace elit21::shopify::graphql {
namespace {

void collect(const Json& node, std::size_t depth, std::size_t maximum_depth,
             std::vector<ShopifyUserError>& output) {
    if (depth > maximum_depth || node.isNull()) return;
    if (node.isObject()) {
        const auto direct = ShopifyUserErrorParser::parse(node);
        output.insert(output.end(), direct.begin(), direct.end());
        json_object_object_foreach(node.raw(), key, value) {
            if (std::string(key) == "userErrors") continue;
            collect(Json(value, false), depth + 1, maximum_depth, output);
        }
    } else if (node.isArray()) {
        for (std::size_t index = 0; index < node.size(); ++index) {
            collect(node.at(index), depth + 1, maximum_depth, output);
        }
    }
}

} // namespace

std::string ShopifyUserError::fieldPath() const {
    std::ostringstream output;
    for (std::size_t index = 0; index < field.size(); ++index) {
        if (index) output << '.';
        output << field[index];
    }
    return output.str();
}

std::string ShopifyUserError::describe() const {
    std::ostringstream output;
    if (!field.empty()) output << fieldPath() << ": ";
    output << message;
    if (!code.empty()) output << " [" << code << ']';
    return output.str();
}

std::vector<ShopifyUserError> ShopifyUserErrorParser::parse(const Json& payload) {
    std::vector<ShopifyUserError> output;
    const auto errors = payload.get("userErrors");
    for (std::size_t index = 0; index < errors.size(); ++index) {
        ShopifyUserError error;
        const auto value = errors.at(index);
        error.message = value.getString("message", "Unknown Shopify user error");
        error.code = value.getString("code");
        const auto field = value.get("field");
        for (std::size_t part = 0; part < field.size(); ++part) {
            error.field.push_back(field.at(part).scalarStringValue());
        }
        output.push_back(std::move(error));
    }
    return output;
}

std::vector<ShopifyUserError> ShopifyUserErrorParser::findRecursively(const Json& payload,
                                                                      std::size_t maximum_depth) {
    std::vector<ShopifyUserError> collected;
    collect(payload, 0, maximum_depth, collected);
    std::vector<ShopifyUserError> output;
    std::set<std::string> identities;
    for (auto& error : collected) {
        const auto identity = util::lower(error.fieldPath() + "|" + error.code + "|" + error.message);
        if (identities.insert(identity).second) output.push_back(std::move(error));
    }
    return output;
}

std::string ShopifyUserErrorParser::join(const std::vector<ShopifyUserError>& errors) {
    std::ostringstream output;
    for (std::size_t index = 0; index < errors.size(); ++index) {
        if (index) output << "; ";
        output << errors[index].describe();
    }
    return output.str();
}

bool ShopifyUserErrorParser::containsCode(const std::vector<ShopifyUserError>& errors,
                                           const std::string& code) {
    const auto expected = util::lower(code);
    for (const auto& error : errors) if (util::lower(error.code) == expected) return true;
    return false;
}

} // namespace elit21::shopify::graphql
