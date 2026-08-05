#include "elit21/aliexpress/requests/DsTradeOrderGetRequest.h"
namespace elit21::aliexpress::requests {transport::AliExpressRequest DsTradeOrderGetRequest::create(std::map<std::string,std::string>p){return transport::AliExpressRequest("aliexpress.ds.trade.order.get",std::move(p),true,std::vector<std::string>{"order_id"});}}
