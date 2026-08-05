#include "elit21/aliexpress/requests/DsImageSearchRequest.h"
namespace elit21::aliexpress::requests {transport::AliExpressRequest DsImageSearchRequest::create(std::map<std::string,std::string>p){return transport::AliExpressRequest("aliexpress.ds.image.search",std::move(p),false,std::vector<std::string>{"image_file_bytes"});}}
