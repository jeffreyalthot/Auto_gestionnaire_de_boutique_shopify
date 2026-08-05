#pragma once
#include "elit21/core/Result.h"
#include "elit21/platform/BusinessComponent.h"
#include "elit21/shopify/ShopifyWebhookRegistry.h"
namespace elit21::shopify::webhooks {class ShopifyWebhookRegistrar final:public platform::BusinessComponent{public:ShopifyWebhookRegistrar();ShopifyWebhookRegistrar(ShopifyClient&client,ShopifyConfig config);void bind(ShopifyClient&client,ShopifyConfig config);Result<ShopifyWebhookRegistrySummary>reconcile(bool dry_run);platform::OperationResult execute(const platform::OperationContext&context)override;private:ShopifyClient*client_{nullptr};ShopifyConfig config_;};}
