#pragma once

#include "elit21/aliexpress/AliExpressModels.h"
#include "elit21/config/Config.h"
#include "elit21/core/Result.h"
#include "elit21/core/Types.h"
#include "elit21/json/Json.h"
#include "elit21/net/HttpClient.h"

#include <map>
#include <string>
#include <vector>

namespace elit21 {

class AliExpressClient {
public:
    AliExpressClient(AliExpressConfig config, HttpClient& http);

    Result<Json> call(const std::string& method,
                      std::map<std::string, std::string> parameters,
                      bool requires_session = true,
                      int max_attempts = 3);
    Result<void> healthCheck();
    Result<AliProduct> getProduct(const std::string& product_id);
    Result<std::vector<AliProduct>> recommendations(int page = 1, int page_size = 20);
    Result<std::vector<FreightQuote>> calculateFreight(const std::string& product_id,
                                                       const std::string& sku_id,
                                                       int quantity,
                                                       const Address& address);
    Result<SupplierOrderResult> placeOrder(const CustomerOrder& order,
                                           const std::string& shipping_service);
    Result<Json> getOrder(const std::string& order_id);
    Result<ShipmentInfo> getTracking(const std::string& order_id,
                                     const std::string& tracking_number,
                                     const std::string& service);
    std::string authorizationUrl(const std::string& redirect_uri, const std::string& state) const;

    const AliExpressConfig& config() const noexcept { return config_; }

private:
    static Json responseResult(const Json& root, const std::string& response_key);
    static double parseRate(const Json& object, const std::string& key, double fallback = 0.0);

    AliExpressConfig config_;
    HttpClient& http_;
};

} // namespace elit21
