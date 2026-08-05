#include "elit21/security/DataRedactor.h"
#include <regex>
namespace elit21::security {
DataRedactor::DataRedactor():platform::BusinessComponent("DataRedactor","secret and PII log redaction",platform::BusinessComponentSpec{"security","redact",{},false,false,4U*1024U*1024U}){}
std::string DataRedactor::redact(const std::string& input){std::string out=input;const std::regex named(R"((access_token|refresh_token|api_key|app_secret|client_secret|password|authorization)(\s*[=:]\s*|\"\s*:\s*\")([^\s,;\"}]+))",std::regex::icase);out=std::regex_replace(out,named,"$1$2[REDACTED]");const std::regex bearer(R"((Bearer\s+)[A-Za-z0-9._~+\-/=]+)",std::regex::icase);out=std::regex_replace(out,bearer,"$1[REDACTED]");const std::regex email(R"(([A-Z0-9._%+-])[A-Z0-9._%+-]*(@[A-Z0-9.-]+\.[A-Z]{2,}))",std::regex::icase);out=std::regex_replace(out,email,"$1***$2");return out;}
platform::OperationResult DataRedactor::execute(const platform::OperationContext&context){auto r=platform::OperationResult::ok("Sensitive fields redacted");r.attributes["redacted_payload"]=redact(context.payload);r.metrics["input_bytes"]=static_cast<double>(context.payload.size());return r;}
}
