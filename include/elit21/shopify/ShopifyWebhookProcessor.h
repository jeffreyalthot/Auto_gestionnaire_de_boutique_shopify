#pragma once

#include "elit21/config/Config.h"
#include "elit21/core/Types.h"
#include "elit21/logging/Logger.h"
#include "elit21/net/HttpServer.h"
#include "elit21/orders/OrderManager.h"
#include "elit21/shopify/ShopifyWebhook.h"
#include "elit21/storage/Database.h"

#include <string>

namespace elit21 {

class ShopifyWebhookProcessor {
public:
    ShopifyWebhookProcessor(ShopifyConfig config,
                            Database& database,
                            OrderManager* order_manager = nullptr,
                            RuntimeCounters* counters = nullptr,
                            Logger* logger = nullptr);

    OutgoingResponse handle(const IncomingRequest& request);
    [[nodiscard]] static std::string taskKindForTopic(const std::string& topic);

private:
    [[nodiscard]] bool shopDomainAccepted(const IncomingRequest& request) const;
    [[nodiscard]] std::string topicFor(const IncomingRequest& request) const;

    ShopifyConfig config_;
    Database& database_;
    OrderManager* order_manager_{nullptr};
    RuntimeCounters* counters_{nullptr};
    Logger* logger_{nullptr};
    ShopifyWebhook webhook_;
};

} // namespace elit21
