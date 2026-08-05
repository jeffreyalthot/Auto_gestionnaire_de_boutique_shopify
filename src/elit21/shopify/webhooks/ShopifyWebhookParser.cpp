#include "elit21/shopify/webhooks/ShopifyWebhookParser.h"
#include "elit21/security/Crypto.h"
#include "elit21/util/StringUtil.h"
namespace elit21::shopify::webhooks {namespace{std::string header(const IncomingRequest&r,const std::string&name){auto it=r.headers.find(name);return it==r.headers.end()?std::string{}:util::trim(it->second);}}ShopifyWebhookParser::ShopifyWebhookParser():platform::BusinessComponent("ShopifyWebhookParser","Typed Shopify webhook request parser",platform::BusinessComponentSpec{"shopify","parse_webhook",{},false,false,16U*1024U*1024U}){}Result<ShopifyWebhook>ShopifyWebhookParser::parse(const IncomingRequest&r)const{if(r.method!="POST")return Result<ShopifyWebhook>::failure("Shopify webhook must use POST");if(r.body.empty())return Result<ShopifyWebhook>::failure("Shopify webhook body is empty");auto payload=Json::parse(r.body);if(!payload||!payload.value().isObject())return Result<ShopifyWebhook>::failure("Shopify webhook body is not a JSON object");auto id=header(r,"x-shopify-webhook-id");if(id.empty())id=crypto::sha256Hex(r.body);ShopifyWebhook event(id,
                         parseTopic(header(r,"x-shopify-topic")),
                         header(r,"x-shopify-shop-domain"),
                         header(r,"x-shopify-api-version"),
                         r.body,
                         payload.take(),
                         header(r,"x-shopify-event-id"),
                         header(r,"x-shopify-triggered-at"),
                         header(r,"x-shopify-name"));auto valid=event.validate();if(!valid)return Result<ShopifyWebhook>::failure(valid.error());return Result<ShopifyWebhook>::success(std::move(event));}platform::OperationResult ShopifyWebhookParser::execute(const platform::OperationContext&context){IncomingRequest r;r.method="POST";r.body=context.payload;r.headers["x-shopify-topic"]="products/update";r.headers["x-shopify-shop-domain"]="example.myshopify.com";r.headers["x-shopify-webhook-id"]="dry-run";auto parsed=parse(r);if(!parsed)return platform::OperationResult::failure("INVALID_WEBHOOK",parsed.error());auto out=platform::OperationResult::ok("Shopify webhook parsed");out.attributes["topic"]=topicName(parsed.value().topic());out.metrics["payload_bytes"]=static_cast<double>(context.payload.size());return out;}}
