#include "elit21/aliexpress/requests/FreightCalculateRequest.h"
namespace elit21::aliexpress::requests {transport::AliExpressRequest FreightCalculateRequest::create(std::map<std::string,std::string>p){return transport::AliExpressRequest("aliexpress.logistics.buyer.freight.calculate",std::move(p),true,std::vector<std::string>{"param_aeop_freight_calculate_for_buyer_d_t_o"});}}
