#include "elit21/network/RequestInterceptor.h"

namespace elit21::network {

RequestInterceptor::RequestInterceptor()
    : platform::BusinessComponent(
          "RequestInterceptor",
          "HTTP, TLS and webhook network transport",
          platform::BusinessComponentSpec{
              "network",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RequestInterceptor::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::network
