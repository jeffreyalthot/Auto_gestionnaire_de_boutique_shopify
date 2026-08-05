#include "elit21/aliexpress/requests/FindProductByIdForDropshipperRequest.h"
namespace elit21::aliexpress::requests {transport::AliExpressRequest FindProductByIdForDropshipperRequest::create(std::map<std::string,std::string>p){return transport::AliExpressRequest("aliexpress.postproduct.redefining.findaeproductbyidfordropshipper",std::move(p),true,std::vector<std::string>{"product_id"});}}
