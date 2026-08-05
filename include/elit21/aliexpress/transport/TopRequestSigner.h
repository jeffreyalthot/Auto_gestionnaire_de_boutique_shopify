#pragma once
#include <map>
#include <string>
namespace elit21::aliexpress::transport {class TopRequestSigner{public:static std::string signMd5(const std::map<std::string,std::string>&parameters,const std::string&secret);static std::string canonicalString(const std::map<std::string,std::string>&parameters,const std::string&secret);};}
