#include "elit21/network/HttpClientPool.h"

namespace elit21::network {

HttpClientPool::HttpClientPool()
    : platform::BusinessComponent(
          "HttpClientPool",
          "HTTP, TLS and webhook network transport",
          platform::BusinessComponentSpec{
              "network",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult HttpClientPool::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::network
