#include "elit21/config/Config.h"
#include "elit21/security/Crypto.h"
#include "elit21/shopify/ShopifyWebhook.h"
#include <fstream>
#include <iostream>
#include <sstream>
int main(int argc,char**argv){if(argc<3){std::cerr<<"usage: webhook_replayer <config> <payload.json>\n";return 2;}auto c=elit21::Config::load(argv[1]);if(!c){std::cerr<<c.error()<<'\n';return 3;}std::ifstream f(argv[2]);if(!f){std::cerr<<"payload not found\n";return 4;}std::ostringstream b;b<<f.rdbuf();elit21::IncomingRequest r;r.method="POST";r.path="/webhooks/shopify/orders-paid";r.body=b.str();r.headers["x-shopify-hmac-sha256"]=elit21::crypto::hmacSha256Base64(c.value().shopify.webhook_secret,r.body);elit21::ShopifyWebhook w(c.value().shopify);auto o=w.parseOrderPaid(r.body);std::cout<<"signature="<<(w.verify(r)?"valid":"invalid")<<'\n';if(!o){std::cerr<<o.error()<<'\n';return 5;}std::cout<<"order="<<o.value().shopify_order_id<<" lines="<<o.value().lines.size()<<'\n';return 0;}
