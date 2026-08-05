#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"

#include <cstddef>
#include <string>
#include <unordered_set>

namespace elit21::shopify::graphql {

class ShopifyGraphqlPaginator {
public:
    explicit ShopifyGraphqlPaginator(std::size_t maximum_pages = 1000);

    [[nodiscard]] Result<void> update(const Json& page_info);
    [[nodiscard]] bool hasNextPage() const noexcept { return has_next_; }
    [[nodiscard]] const std::string& cursor() const noexcept { return cursor_; }
    [[nodiscard]] std::size_t pageCount() const noexcept { return page_count_; }
    [[nodiscard]] Json variables(int first = 100) const;
    void reset();

private:
    bool has_next_{true};
    std::string cursor_;
    std::size_t page_count_{0};
    std::size_t maximum_pages_{1000};
    std::unordered_set<std::string> seen_cursors_;
};

} // namespace elit21::shopify::graphql
