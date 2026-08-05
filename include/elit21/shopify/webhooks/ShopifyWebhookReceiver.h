#pragma once
#include "elit21/core/Result.h"
#include "elit21/net/HttpServer.h"
#include "elit21/platform/BusinessComponent.h"
#include "elit21/shopify/webhooks/ShopifyWebhookDispatcher.h"
#include "elit21/shopify/webhooks/ShopifyWebhookParser.h"
#include "elit21/shopify/webhooks/ShopifyWebhookSignatureVerifier.h"
namespace elit21::shopify::webhooks {class ShopifyWebhookReceiver final:public platform::BusinessComponent{public:ShopifyWebhookReceiver();ShopifyWebhookReceiver(std::string secret,ShopifyWebhookDispatcher&dispatcher);void bind(std::string secret,ShopifyWebhookDispatcher&dispatcher);OutgoingResponse receive(const IncomingRequest&request);platform::OperationResult execute(const platform::OperationContext&context)override;private:ShopifyWebhookSignatureVerifier verifier_;ShopifyWebhookParser parser_;ShopifyWebhookDispatcher*dispatcher_{nullptr};};}
