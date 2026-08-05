#include "elit21/aliexpress/AliExpressClient.h"
#include "elit21/config/Config.h"
#include "elit21/net/HttpClient.h"
#include "elit21/shopify/ShopifyClient.h"
#include <iostream>
int main(int argc,char**argv){auto c=elit21::Config::load(argc>1?argv[1]:"config/app.json");if(!c){std::cerr<<c.error()<<'\n';return 2;}elit21::HttpClient h;h.setTimeoutSeconds(c.value().network.request_timeout_seconds);h.setConnectTimeoutSeconds(c.value().network.connect_timeout_seconds);h.setTlsVerification(c.value().network.verify_tls);elit21::ShopifyClient s(c.value().shopify,h);elit21::AliExpressClient a(c.value().aliexpress,h);bool ok=true;if(c.value().shopify.shop.empty()||c.value().shopify.access_token.empty())std::cout<<"Shopify: SKIPPED (credentials missing)\n";else{auto r=s.healthCheck();std::cout<<"Shopify: "<<(r?"OK":r.error())<<'\n';ok=ok&&bool(r);}if(c.value().aliexpress.app_key.empty()||c.value().aliexpress.app_secret.empty())std::cout<<"AliExpress: SKIPPED (credentials missing)\n";else{auto r=a.healthCheck();std::cout<<"AliExpress: "<<(r?"OK":r.error())<<'\n';ok=ok&&bool(r);}std::cout<<"configuration="<<c.value().sanitizedSummary().dump()<<'\n';return ok?0:1;}
