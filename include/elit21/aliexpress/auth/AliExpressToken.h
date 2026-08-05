#pragma once
#include <cstdint>
#include <string>
namespace elit21::aliexpress::auth {struct AliExpressToken{std::string access_token,refresh_token,account_id;std::int64_t expires_in_seconds{0},refresh_expires_in_seconds{0};[[nodiscard]]bool valid()const{return !access_token.empty()&&expires_in_seconds>0;}};}
