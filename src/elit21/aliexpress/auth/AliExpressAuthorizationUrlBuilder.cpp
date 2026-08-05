#include "elit21/aliexpress/auth/AliExpressAuthorizationUrlBuilder.h"
#include "elit21/aliexpress/AliExpressEndpoints.h"
#include "elit21/util/StringUtil.h"
namespace elit21::aliexpress::auth {std::string AliExpressAuthorizationUrlBuilder::build(const std::string&c,const std::string&r,const std::string&s,bool f){return std::string(AliExpressEndpoints::oauthAuthorize())+"?response_type=code&force_auth="+(f?"true":"false")+"&redirect_uri="+util::urlEncode(r)+"&client_id="+util::urlEncode(c)+"&state="+util::urlEncode(s);}}
