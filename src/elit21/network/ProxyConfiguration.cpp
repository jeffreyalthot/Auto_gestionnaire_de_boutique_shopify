#include "elit21/network/ProxyConfiguration.h"

namespace elit21::network {

ProxyConfiguration::ProxyConfiguration()
    : platform::BusinessComponent(
          "ProxyConfiguration",
          "HTTP, TLS and webhook network transport",
          platform::BusinessComponentSpec{
              "network",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProxyConfiguration::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::network
