#include "elit21/aliexpress/requests/PlaceOrderRequest.h"
namespace elit21::aliexpress::requests {transport::AliExpressRequest PlaceOrderRequest::create(std::map<std::string,std::string>p){return transport::AliExpressRequest("aliexpress.trade.buy.placeorder",std::move(p),true,std::vector<std::string>{"param_place_order_request4_open_api_d_t_o"});}}
