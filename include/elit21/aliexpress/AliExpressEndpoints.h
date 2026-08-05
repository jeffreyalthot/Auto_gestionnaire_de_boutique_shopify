#pragma once
#include <string>
namespace elit21::aliexpress {
class AliExpressEndpoints{public:static constexpr const char* internationalRest()noexcept{return "https://api-sg.aliexpress.com/rest";}static constexpr const char* topGateway()noexcept{return "https://eco.taobao.com/router/rest";}static constexpr const char* oauthAuthorize()noexcept{return "https://api-sg.aliexpress.com/oauth/authorize";}static constexpr const char* tokenCreate()noexcept{return "https://api-sg.aliexpress.com/rest/auth/token/create";}static constexpr const char* tokenRefresh()noexcept{return "https://api-sg.aliexpress.com/rest/auth/token/refresh";}static bool secure(const std::string&url);};
}
