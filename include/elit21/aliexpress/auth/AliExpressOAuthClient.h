#pragma once
#include "elit21/aliexpress/auth/AliExpressToken.h"
#include "elit21/core/Result.h"
#include "elit21/net/HttpClient.h"
#include <string>
namespace elit21::aliexpress::auth {class AliExpressOAuthClient{public:explicit AliExpressOAuthClient(HttpClient&http):http_(http){}Result<AliExpressToken>createToken(const std::string&app_key,const std::string&app_secret,const std::string&code);Result<AliExpressToken>refreshToken(const std::string&app_key,const std::string&app_secret,const std::string&refresh_token);private:Result<AliExpressToken>parse(const HttpResponse&response);HttpClient&http_;};}
