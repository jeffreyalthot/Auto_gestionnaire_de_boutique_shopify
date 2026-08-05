#include "elit21/network/FormEncoder.h"

namespace elit21::network {

FormEncoder::FormEncoder()
    : platform::BusinessComponent(
          "FormEncoder",
          "HTTP, TLS and webhook network transport",
          platform::BusinessComponentSpec{
              "network",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FormEncoder::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::network
