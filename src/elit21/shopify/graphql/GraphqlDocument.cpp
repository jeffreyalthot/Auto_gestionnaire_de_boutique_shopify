#include "elit21/shopify/graphql/GraphqlDocument.h"

#include "elit21/platform/ManagedComponent.h"
#include "elit21/util/StringUtil.h"

#include <cctype>
#include <regex>
#include <sstream>
#include <unordered_set>

namespace elit21::shopify::graphql {
namespace {

std::string firstToken(const std::string& source) {
    const auto stripped = GraphqlDocument::stripComments(source);
    const auto trimmed = util::trim(stripped);
    const auto end = trimmed.find_first_of(" \t\r\n{");
    return util::lower(trimmed.substr(0, end));
}

bool validName(const std::string& value) {
    static const std::regex pattern(R"(^[_A-Za-z][_0-9A-Za-z]*$)");
    return std::regex_match(value, pattern);
}

} // namespace

GraphqlDocument::GraphqlDocument(std::string operation_name, std::string source)
    : operation_name_(std::move(operation_name)), source_(std::move(source)) {}

GraphqlOperationType GraphqlDocument::type() const noexcept {
    const auto token = firstToken(source_);
    if (token == "query") return GraphqlOperationType::query;
    if (token == "mutation") return GraphqlOperationType::mutation;
    return GraphqlOperationType::unknown;
}

std::string GraphqlDocument::stripComments(std::string_view source) {
    std::string output;
    output.reserve(source.size());
    bool in_string = false;
    bool escaped = false;
    bool in_comment = false;
    for (char ch : source) {
        if (in_comment) {
            if (ch == '\n') {
                in_comment = false;
                output.push_back(ch);
            }
            continue;
        }
        if (in_string) {
            output.push_back(ch);
            if (escaped) escaped = false;
            else if (ch == '\\') escaped = true;
            else if (ch == '"') in_string = false;
            continue;
        }
        if (ch == '#') {
            in_comment = true;
            continue;
        }
        if (ch == '"') in_string = true;
        output.push_back(ch);
    }
    return output;
}

std::string GraphqlDocument::normalizedSource() const {
    const auto stripped = stripComments(source_);
    std::string output;
    output.reserve(stripped.size());
    bool in_string = false;
    bool escaped = false;
    bool whitespace = false;
    for (char ch : stripped) {
        if (in_string) {
            output.push_back(ch);
            if (escaped) escaped = false;
            else if (ch == '\\') escaped = true;
            else if (ch == '"') in_string = false;
            continue;
        }
        if (ch == '"') {
            if (whitespace && !output.empty()) output.push_back(' ');
            whitespace = false;
            in_string = true;
            output.push_back(ch);
        } else if (std::isspace(static_cast<unsigned char>(ch))) {
            whitespace = true;
        } else {
            if (whitespace && !output.empty() && output.back() != '{' && output.back() != '(' &&
                ch != '}' && ch != ')' && ch != ':' && ch != ',') output.push_back(' ');
            whitespace = false;
            output.push_back(ch);
        }
    }
    return util::trim(output);
}

std::vector<std::string> GraphqlDocument::declaredVariables() const {
    std::vector<std::string> result;
    std::unordered_set<std::string> seen;
    const auto source = stripComments(source_);
    static const std::regex variable(R"(\$([_A-Za-z][_0-9A-Za-z]*)\s*:)");
    for (auto it = std::sregex_iterator(source.begin(), source.end(), variable);
         it != std::sregex_iterator(); ++it) {
        const auto value = (*it)[1].str();
        if (seen.insert(value).second) result.push_back(value);
    }
    return result;
}

std::string GraphqlDocument::fingerprint() const {
    return std::to_string(platform::stableHash(operation_name_ + "\n" + normalizedSource()));
}

Result<void> GraphqlDocument::validate() const {
    if (!validName(operation_name_)) {
        return Result<void>::failure("GraphQL operation name is empty or invalid");
    }
    const auto normalized = normalizedSource();
    if (normalized.empty()) return Result<void>::failure("GraphQL document is empty");
    if (type() == GraphqlOperationType::unknown) {
        return Result<void>::failure("GraphQL document must start with query or mutation");
    }
    const std::regex declared("^(query|mutation)\\s+" + operation_name_ + R"((\s|\(|\{))");
    if (!std::regex_search(normalized, declared)) {
        return Result<void>::failure("GraphQL operation name does not match the document declaration");
    }

    std::vector<char> stack;
    bool in_string = false;
    bool escaped = false;
    for (char ch : stripComments(source_)) {
        if (in_string) {
            if (escaped) escaped = false;
            else if (ch == '\\') escaped = true;
            else if (ch == '"') in_string = false;
            continue;
        }
        if (ch == '"') {
            in_string = true;
            continue;
        }
        if (ch == '{' || ch == '(' || ch == '[') stack.push_back(ch);
        else if (ch == '}' || ch == ')' || ch == ']') {
            if (stack.empty()) return Result<void>::failure("Unbalanced GraphQL delimiters");
            const auto open = stack.back();
            stack.pop_back();
            if ((open == '{' && ch != '}') || (open == '(' && ch != ')') ||
                (open == '[' && ch != ']')) {
                return Result<void>::failure("Mismatched GraphQL delimiters");
            }
        }
    }
    if (in_string) return Result<void>::failure("Unterminated GraphQL string literal");
    if (!stack.empty()) return Result<void>::failure("Unbalanced GraphQL delimiters");
    if (normalized.find('{') == std::string::npos) {
        return Result<void>::failure("GraphQL selection set is missing");
    }
    return Result<void>::success();
}

} // namespace elit21::shopify::graphql
