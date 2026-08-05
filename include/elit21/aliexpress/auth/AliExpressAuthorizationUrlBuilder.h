#pragma once
#include <string>
namespace elit21::aliexpress::auth {class AliExpressAuthorizationUrlBuilder{public:static std::string build(const std::string&client_id,const std::string&redirect_uri,const std::string&state,bool force_auth=true);};}
