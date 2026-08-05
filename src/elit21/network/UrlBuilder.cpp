#include "elit21/network/UrlBuilder.h"

namespace elit21::network {

UrlBuilder::UrlBuilder()
    : platform::BusinessComponent(
          "UrlBuilder",
          "HTTP, TLS and webhook network transport",
          platform::BusinessComponentSpec{
              "network",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult UrlBuilder::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::network
