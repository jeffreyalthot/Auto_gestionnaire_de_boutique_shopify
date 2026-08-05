#pragma once
#include "elit21/core/Result.h"
#include <map>
#include <string>
#include <vector>
namespace elit21::aliexpress::transport {
class AliExpressRequest{public:AliExpressRequest(std::string method,std::map<std::string,std::string> parameters={},bool requires_session=true,std::vector<std::string> required_fields={});[[nodiscard]]const std::string&method()const{return method_;}[[nodiscard]]const std::map<std::string,std::string>&parameters()const{return parameters_;}[[nodiscard]]bool requiresSession()const{return requires_session_;}[[nodiscard]]Result<void>validate()const;void set(std::string key,std::string value);private:std::string method_;std::map<std::string,std::string>parameters_;bool requires_session_;std::vector<std::string>required_fields_;};
}
