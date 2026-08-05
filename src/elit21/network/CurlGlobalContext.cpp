#include "elit21/network/CurlGlobalContext.h"

namespace elit21::network {

CurlGlobalContext::CurlGlobalContext()
    : platform::BusinessComponent(
          "CurlGlobalContext",
          "HTTP, TLS and webhook network transport",
          platform::BusinessComponentSpec{
              "network",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CurlGlobalContext::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::network
