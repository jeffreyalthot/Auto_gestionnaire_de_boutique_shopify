#include "elit21/aliexpress/requests/DsAddInfoRequest.h"
namespace elit21::aliexpress::requests {transport::AliExpressRequest DsAddInfoRequest::create(std::map<std::string,std::string>p){return transport::AliExpressRequest("aliexpress.ds.add.info",std::move(p),true,std::vector<std::string>{"param0"});}}
