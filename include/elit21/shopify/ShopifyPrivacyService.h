#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/storage/Database.h"

#include <string>
#include <vector>

namespace elit21::shopify {

struct ShopifyPrivacyResult {
    std::string request_id;
    std::string topic;
    std::string status;
    std::string export_path;
    int affected_orders{0};
    bool duplicate{false};

    [[nodiscard]] Json toJson() const;
};

class ShopifyPrivacyService {
public:
    ShopifyPrivacyService(Database& database, std::string data_directory);

    Result<ShopifyPrivacyResult> processTask(const std::string& task_kind,
                                             const Json& task_payload);

private:
    struct ParsedRequest {
        std::string request_id;
        std::string topic;
        std::string shop_domain;
        std::string customer_id;
        std::string payload_hash;
        std::vector<std::string> order_ids;
        Json body;
    };

    Result<ParsedRequest> parseRequest(const std::string& task_kind,
                                       const Json& task_payload) const;
    Result<ShopifyPrivacyResult> exportCustomerData(const ParsedRequest& request);
    Result<ShopifyPrivacyResult> redactCustomerData(const ParsedRequest& request);
    Result<ShopifyPrivacyResult> redactShopData(const ParsedRequest& request);
    static std::vector<std::string> parseOrderIds(const Json& body);
    static std::string atomicWrite(const std::string& directory,
                                   const std::string& file_name,
                                   const std::string& body);

    Database& database_;
    std::string data_directory_;
};

} // namespace elit21::shopify
