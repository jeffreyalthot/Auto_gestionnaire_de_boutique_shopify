#include "elit21/network/NetworkDiagnostics.h"

namespace elit21::network {

NetworkDiagnostics::NetworkDiagnostics()
    : platform::BusinessComponent(
          "NetworkDiagnostics",
          "HTTP, TLS and webhook network transport",
          platform::BusinessComponentSpec{
              "network",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult NetworkDiagnostics::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::network
