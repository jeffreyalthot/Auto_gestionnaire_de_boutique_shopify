#pragma once
#include "elit21/core/Result.h"
#include "elit21/platform/BusinessComponent.h"
#include "elit21/shopify/webhooks/ShopifyWebhook.h"
#include <functional>
#include <map>
namespace elit21::shopify::webhooks {class ShopifyWebhookDispatcher final:public platform::BusinessComponent{public:using Handler=std::function<Result<void>(const ShopifyWebhook&)>;ShopifyWebhookDispatcher();void registerHandler(ShopifyWebhookTopic topic,Handler handler);Result<void>dispatch(const ShopifyWebhook&event)const;[[nodiscard]]std::size_t handlerCount()const noexcept{return handlers_.size();}platform::OperationResult execute(const platform::OperationContext&context)override;private:std::map<ShopifyWebhookTopic,Handler>handlers_;};}
