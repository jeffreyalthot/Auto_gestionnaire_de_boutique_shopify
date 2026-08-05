#pragma once

#include "elit21/core/Result.h"

#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace elit21::shopify::graphql {

enum class GraphqlOperationType { query, mutation, unknown };

class GraphqlDocument {
public:
    GraphqlDocument(std::string operation_name, std::string source);

    [[nodiscard]] const std::string& operationName() const noexcept { return operation_name_; }
    [[nodiscard]] const std::string& source() const noexcept { return source_; }
    [[nodiscard]] GraphqlOperationType type() const noexcept;
    [[nodiscard]] bool isMutation() const noexcept { return type() == GraphqlOperationType::mutation; }
    [[nodiscard]] bool isQuery() const noexcept { return type() == GraphqlOperationType::query; }
    [[nodiscard]] std::vector<std::string> declaredVariables() const;
    [[nodiscard]] std::string normalizedSource() const;
    [[nodiscard]] std::string fingerprint() const;
    [[nodiscard]] Result<void> validate() const;

    static std::string stripComments(std::string_view source);

private:
    std::string operation_name_;
    std::string source_;
};

} // namespace elit21::shopify::graphql
