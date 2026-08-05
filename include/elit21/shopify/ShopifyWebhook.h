#pragma once
#include "elit21/config/Config.h"
#include "elit21/core/Result.h"
#include "elit21/core/Types.h"
#include "elit21/net/HttpServer.h"
#include <functional>
namespace elit21 {class ShopifyWebhook{public:explicit ShopifyWebhook(ShopifyConfig c):config_(std::move(c)){}bool verify(const IncomingRequest&r)const;Result<CustomerOrder> parseOrderPaid(const std::string&body)const;private:ShopifyConfig config_;};}
