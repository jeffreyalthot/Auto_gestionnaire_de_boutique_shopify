#include "elit21/aliexpress/AliExpressMethodNames.h"
#include "elit21/aliexpress/transport/TopRequestSigner.h"
#include "elit21/config/Config.h"
#include <iostream>
#include <map>
int main(int argc,char**argv){auto c=elit21::Config::load(argc>1?argv[1]:"config/app.json");if(!c){std::cerr<<c.error()<<'\n';return 2;}const std::string method=argc>2?argv[2]:elit21::aliexpress::AliExpressMethodNames::productGet();if(!elit21::aliexpress::AliExpressMethodNames::supported(method)){std::cerr<<"unsupported method\n";return 3;}std::map<std::string,std::string> p{{"app_key",c.value().aliexpress.app_key},{"method",method},{"format","json"},{"v","2.0"},{"sign_method","md5"}};std::cout<<"method="<<method<<"\nsign="<<elit21::aliexpress::transport::TopRequestSigner::signMd5(p,c.value().aliexpress.app_secret)<<'\n';return 0;}
