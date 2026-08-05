#include "elit21/aliexpress/requests/DsProductGetRequest.h"
namespace elit21::aliexpress::requests {transport::AliExpressRequest DsProductGetRequest::create(std::map<std::string,std::string>p){return transport::AliExpressRequest("aliexpress.ds.product.get",std::move(p),true,std::vector<std::string>{"product_id"});}}
