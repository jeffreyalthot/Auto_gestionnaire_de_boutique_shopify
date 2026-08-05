#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/platform/BusinessComponent.h"

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

namespace elit21::shopify::bulk {

struct BulkJsonlRecord {
    std::size_t line{0};
    std::string id;
    std::string parent_id;
    Json value{Json::object()};
};

struct BulkJsonlStatistics {
    std::size_t lines{0};
    std::size_t objects{0};
    std::size_t children{0};
    std::size_t invalid{0};
    std::size_t bytes{0};
};

class BulkJsonlParser final : public platform::BusinessComponent {
public:
    using Consumer = std::function<Result<void>(const BulkJsonlRecord&)>;

    BulkJsonlParser();
    Result<BulkJsonlStatistics> parseText(const std::string& jsonl, const Consumer& consumer) const;
    Result<BulkJsonlStatistics> parseFile(const std::string& path, const Consumer& consumer) const;
    Result<std::vector<BulkJsonlRecord>> readAll(const std::string& path,
                                                 std::size_t maximum_records = 100000) const;
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::shopify::bulk
