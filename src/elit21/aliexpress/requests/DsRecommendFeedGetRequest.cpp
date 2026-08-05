#include "elit21/aliexpress/requests/DsRecommendFeedGetRequest.h"
namespace elit21::aliexpress::requests {transport::AliExpressRequest DsRecommendFeedGetRequest::create(std::map<std::string,std::string>p){return transport::AliExpressRequest("aliexpress.ds.recommend.feed.get",std::move(p),false,std::vector<std::string>{});}}
