#include "elit21/network/HttpResponse.h"

namespace elit21::network {

HttpResponse::HttpResponse()
    : platform::BusinessComponent(
          "HttpResponse",
          "HTTP, TLS and webhook network transport",
          platform::BusinessComponentSpec{
              "network",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult HttpResponse::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::network
