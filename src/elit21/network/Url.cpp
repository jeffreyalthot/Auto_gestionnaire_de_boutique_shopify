#include "elit21/network/Url.h"

namespace elit21::network {

Url::Url()
    : platform::BusinessComponent(
          "Url",
          "HTTP, TLS and webhook network transport",
          platform::BusinessComponentSpec{
              "network",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Url::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::network
