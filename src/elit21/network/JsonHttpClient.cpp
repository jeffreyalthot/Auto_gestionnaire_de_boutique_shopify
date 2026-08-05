#include "elit21/network/JsonHttpClient.h"

namespace elit21::network {

JsonHttpClient::JsonHttpClient()
    : platform::BusinessComponent(
          "JsonHttpClient",
          "HTTP, TLS and webhook network transport",
          platform::BusinessComponentSpec{
              "network",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult JsonHttpClient::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::network
