#include "elit21/aliexpress/requests/TrackingInfoQueryRequest.h"
namespace elit21::aliexpress::requests {transport::AliExpressRequest TrackingInfoQueryRequest::create(std::map<std::string,std::string>p){return transport::AliExpressRequest("aliexpress.logistics.ds.trackinginfo.query",std::move(p),true,std::vector<std::string>{"order_id"});}}
