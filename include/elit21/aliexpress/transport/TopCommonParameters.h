#pragma once
#include <map>
#include <string>
namespace elit21::aliexpress::transport {struct TopCommonParameters{std::string app_key,session,timestamp,format{"json"},version{"2.0"},sign_method{"md5"};bool simplify{true};[[nodiscard]]std::map<std::string,std::string>toMap()const;};}
