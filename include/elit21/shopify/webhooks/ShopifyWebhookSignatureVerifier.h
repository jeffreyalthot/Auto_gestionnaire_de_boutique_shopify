#pragma once
#include "elit21/net/HttpServer.h"
#include "elit21/platform/BusinessComponent.h"
#include <string>
namespace elit21::shopify::webhooks {class ShopifyWebhookSignatureVerifier final:public platform::BusinessComponent{public:ShopifyWebhookSignatureVerifier();explicit ShopifyWebhookSignatureVerifier(std::string secret);void setSecret(std::string secret);[[nodiscard]]bool verify(const IncomingRequest&request)const;[[nodiscard]]bool verify(const std::string&body,const std::string&signature)const;platform::OperationResult execute(const platform::OperationContext&context)override;private:std::string secret_;};}
