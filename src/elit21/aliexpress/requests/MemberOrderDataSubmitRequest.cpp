#include "elit21/aliexpress/requests/MemberOrderDataSubmitRequest.h"
namespace elit21::aliexpress::requests {transport::AliExpressRequest MemberOrderDataSubmitRequest::create(std::map<std::string,std::string>p){return transport::AliExpressRequest("aliexpress.ds.member.orderdata.submit",std::move(p),true,std::vector<std::string>{"order_data"});}}
