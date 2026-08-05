#include "elit21/network/HeaderCollection.h"

namespace elit21::network {

HeaderCollection::HeaderCollection()
    : platform::BusinessComponent(
          "HeaderCollection",
          "HTTP, TLS and webhook network transport",
          platform::BusinessComponentSpec{
              "network",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult HeaderCollection::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::network
