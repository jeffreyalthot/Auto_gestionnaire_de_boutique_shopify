#pragma once
#include "elit21/core/Result.h"
#include "elit21/platform/BusinessComponent.h"
#include "elit21/shopify/webhooks/ShopifyWebhookHandlerSupport.h"
namespace elit21::shopify::webhooks {class AppUninstalledWebhookHandler final:public platform::BusinessComponent{public:AppUninstalledWebhookHandler();explicit AppUninstalledWebhookHandler(Database&database);void bind(Database&database)noexcept{database_=&database;}Result<void>handle(const ShopifyWebhook&event);platform::OperationResult execute(const platform::OperationContext&context)override;private:Database*database_{nullptr};};}
