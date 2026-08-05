#include "elit21/aliexpress/requests/TradeDsOrderGetRequest.h"
namespace elit21::aliexpress::requests {transport::AliExpressRequest TradeDsOrderGetRequest::create(std::map<std::string,std::string>p){return transport::AliExpressRequest("aliexpress.trade.ds.order.get",std::move(p),true,std::vector<std::string>{"single_order_query"});}}
