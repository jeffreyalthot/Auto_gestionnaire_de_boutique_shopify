#include "elit21/aliexpress/transport/TopCommonParameters.h"
namespace elit21::aliexpress::transport {std::map<std::string,std::string>TopCommonParameters::toMap()const{return{{"app_key",app_key},{"session",session},{"timestamp",timestamp},{"format",format},{"v",version},{"sign_method",sign_method},{"simplify",simplify?"true":"false"}};}}
