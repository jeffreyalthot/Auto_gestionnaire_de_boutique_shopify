#include "elit21/network/MultipartBuilder.h"

namespace elit21::network {

MultipartBuilder::MultipartBuilder()
    : platform::BusinessComponent(
          "MultipartBuilder",
          "HTTP, TLS and webhook network transport",
          platform::BusinessComponentSpec{
              "network",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult MultipartBuilder::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::network
