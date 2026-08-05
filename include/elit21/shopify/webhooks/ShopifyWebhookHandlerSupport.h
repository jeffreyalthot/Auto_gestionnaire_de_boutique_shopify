#pragma once
#include "elit21/core/Result.h"
#include "elit21/shopify/webhooks/ShopifyWebhook.h"
#include "elit21/storage/Database.h"
#include <initializer_list>
namespace elit21::shopify::webhooks {Result<void>persistWebhookTask(Database&database,const ShopifyWebhook&event,std::initializer_list<ShopifyWebhookTopic>accepted,int priority=10);}
