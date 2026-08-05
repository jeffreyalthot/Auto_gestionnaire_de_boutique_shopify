#pragma once
#include "elit21/aliexpress/transport/AliExpressRequest.h"
#include <map>
#include <string>
namespace elit21::aliexpress::requests {class DsAddInfoRequest{public:static transport::AliExpressRequest create(std::map<std::string,std::string> parameters);};}
