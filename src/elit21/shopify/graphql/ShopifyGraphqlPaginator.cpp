#include "elit21/shopify/graphql/ShopifyGraphqlPaginator.h"

#include <algorithm>

namespace elit21::shopify::graphql {

ShopifyGraphqlPaginator::ShopifyGraphqlPaginator(std::size_t maximum_pages)
    : maximum_pages_(std::max<std::size_t>(1, maximum_pages)) {}

Result<void> ShopifyGraphqlPaginator::update(const Json& page_info) {
    if (!page_info.isObject()) return Result<void>::failure("GraphQL pageInfo must be an object");
    if (page_count_ >= maximum_pages_) {
        has_next_ = false;
        return Result<void>::failure("GraphQL pagination page limit reached");
    }
    ++page_count_;
    const bool next = page_info.getBool("hasNextPage", false);
    const auto next_cursor = page_info.getString("endCursor");
    if (next && next_cursor.empty()) {
        has_next_ = false;
        return Result<void>::failure("GraphQL pagination reports another page without an endCursor");
    }
    if (next && !seen_cursors_.insert(next_cursor).second) {
        has_next_ = false;
        return Result<void>::failure("GraphQL pagination cursor repeated; loop prevented");
    }
    cursor_ = next_cursor;
    has_next_ = next;
    return Result<void>::success();
}

Json ShopifyGraphqlPaginator::variables(int first) const {
    Json output = Json::object();
    output.set("first", std::clamp(first, 1, 250));
    if (!cursor_.empty()) output.set("after", cursor_);
    return output;
}

void ShopifyGraphqlPaginator::reset() {
    has_next_ = true;
    cursor_.clear();
    page_count_ = 0;
    seen_cursors_.clear();
}

} // namespace elit21::shopify::graphql
