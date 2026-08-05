#pragma once
#include "elit21/core/Result.h"
#include "elit21/net/HttpServer.h"
#include "elit21/platform/BusinessComponent.h"
#include "elit21/shopify/webhooks/ShopifyWebhook.h"
namespace elit21::shopify::webhooks {class ShopifyWebhookParser final:public platform::BusinessComponent{public:ShopifyWebhookParser();Result<ShopifyWebhook>parse(const IncomingRequest&request)const;platform::OperationResult execute(const platform::OperationContext&context)override;};}
