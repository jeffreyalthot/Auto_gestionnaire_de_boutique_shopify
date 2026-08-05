#include "elit21/aliexpress/transport/TopRequestSigner.h"
#include "elit21/security/Crypto.h"
namespace elit21::aliexpress::transport {std::string TopRequestSigner::canonicalString(const std::map<std::string,std::string>&p,const std::string&s){std::string out=s;for(const auto&[k,v]:p)if(k!="sign"&&!v.empty())out+=k+v;out+=s;return out;}std::string TopRequestSigner::signMd5(const std::map<std::string,std::string>&p,const std::string&s){return crypto::md5Hex(canonicalString(p,s),true);}}
