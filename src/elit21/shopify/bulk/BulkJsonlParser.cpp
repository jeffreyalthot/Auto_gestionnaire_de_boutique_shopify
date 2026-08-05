#include "elit21/shopify/bulk/BulkJsonlParser.h"

#include "elit21/util/StringUtil.h"

#include <fstream>
#include <sstream>

namespace elit21::shopify::bulk {
namespace {

Result<BulkJsonlStatistics> parseStream(std::istream& input, const BulkJsonlParser::Consumer& consumer) {
    BulkJsonlStatistics statistics;
    std::string line;
    while (std::getline(input, line)) {
        ++statistics.lines;
        statistics.bytes += line.size() + 1;
        if (util::trim(line).empty()) continue;
        auto parsed = Json::parse(line);
        if (!parsed || !parsed.value().isObject()) {
            ++statistics.invalid;
            return Result<BulkJsonlStatistics>::failure(
                "Invalid Shopify bulk JSONL at line " + std::to_string(statistics.lines));
        }
        BulkJsonlRecord record;
        record.line = statistics.lines;
        record.value = parsed.take();
        record.id = record.value.getScalarString("id");
        record.parent_id = record.value.getScalarString("__parentId");
        if (!record.parent_id.empty()) ++statistics.children;
        if (consumer) {
            auto accepted = consumer(record);
            if (!accepted) return Result<BulkJsonlStatistics>::failure(
                "Bulk JSONL consumer rejected line " + std::to_string(record.line) + ": " + accepted.error());
        }
        ++statistics.objects;
    }
    return Result<BulkJsonlStatistics>::success(statistics);
}

} // namespace

BulkJsonlParser::BulkJsonlParser()
    : platform::BusinessComponent("BulkJsonlParser", "Streaming Shopify bulk JSONL parser",
          platform::BusinessComponentSpec{"shopify", "parse_bulk_jsonl", {}, false, false,
                                          256U * 1024U * 1024U}) {}

Result<BulkJsonlStatistics> BulkJsonlParser::parseText(const std::string& jsonl,
                                                        const Consumer& consumer) const {
    std::istringstream input(jsonl);
    return parseStream(input, consumer);
}

Result<BulkJsonlStatistics> BulkJsonlParser::parseFile(const std::string& path,
                                                        const Consumer& consumer) const {
    std::ifstream input(path, std::ios::binary);
    if (!input) return Result<BulkJsonlStatistics>::failure("Cannot open Shopify bulk JSONL file: " + path);
    return parseStream(input, consumer);
}

Result<std::vector<BulkJsonlRecord>> BulkJsonlParser::readAll(const std::string& path,
                                                               std::size_t maximum_records) const {
    std::vector<BulkJsonlRecord> records;
    auto parsed = parseFile(path, [&](const BulkJsonlRecord& record) {
        if (records.size() >= maximum_records) return Result<void>::failure("Bulk record limit reached");
        records.push_back(record);
        return Result<void>::success();
    });
    if (!parsed) return Result<std::vector<BulkJsonlRecord>>::failure(parsed.error());
    return Result<std::vector<BulkJsonlRecord>>::success(std::move(records));
}

platform::OperationResult BulkJsonlParser::execute(const platform::OperationContext& context) {
    auto parsed = parseText(context.payload, {});
    if (!parsed) return platform::OperationResult::failure("INVALID_BULK_JSONL", parsed.error());
    auto result = platform::OperationResult::ok("Shopify bulk JSONL parsed");
    result.metrics["lines"] = static_cast<double>(parsed.value().lines);
    result.metrics["objects"] = static_cast<double>(parsed.value().objects);
    result.metrics["children"] = static_cast<double>(parsed.value().children);
    result.metrics["bytes"] = static_cast<double>(parsed.value().bytes);
    return result;
}

} // namespace elit21::shopify::bulk
