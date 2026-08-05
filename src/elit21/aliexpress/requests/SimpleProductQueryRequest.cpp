#include "elit21/aliexpress/requests/SimpleProductQueryRequest.h"
namespace elit21::aliexpress::requests {transport::AliExpressRequest SimpleProductQueryRequest::create(std::map<std::string,std::string>p){return transport::AliExpressRequest("aliexpress.offer.ds.product.simplequery",std::move(p),true,std::vector<std::string>{"product_id"});}}
