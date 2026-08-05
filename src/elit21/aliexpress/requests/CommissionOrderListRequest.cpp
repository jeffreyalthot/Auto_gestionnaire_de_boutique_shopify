#include "elit21/aliexpress/requests/CommissionOrderListRequest.h"
namespace elit21::aliexpress::requests {transport::AliExpressRequest CommissionOrderListRequest::create(std::map<std::string,std::string>p){return transport::AliExpressRequest("aliexpress.ds.commissionorder.listbyindex",std::move(p),false,std::vector<std::string>{"page_no"});}}
