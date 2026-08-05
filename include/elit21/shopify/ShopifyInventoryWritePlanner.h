#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/shopify/ShopifyModels.h"

#include <cstddef>
#include <string>
#include <vector>

namespace elit21::shopify {

struct ShopifyInventoryWriteBatch {
    std::size_t offset{0};
    std::size_t count{0};
    std::string idempotency_key;
    Json variables{Json::object()};
    bool compare_and_set{true};

    [[nodiscard]] Json toJson() const;
};

class ShopifyInventoryWritePlanner {
public:
    static constexpr std::size_t maximum_input_array_size = 250;

    [[nodiscard]] static Result<std::vector<ShopifyInventoryWriteBatch>> build(
        const std::string& location_gid,
        const std::vector<ShopifyInventoryQuantityUpdate>& updates,
        const std::string& base_idempotency_key,
        const std::string& reference_document_uri,
        bool allow_unchecked = false);

    [[nodiscard]] static const std::string& mutationDocument();
    [[nodiscard]] static Result<void> auditMutationDocument(const std::string& document);
};

} // namespace elit21::shopify
