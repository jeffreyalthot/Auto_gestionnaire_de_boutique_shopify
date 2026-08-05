#include "elit21/risk/LossPreventionPolicy.h"

namespace elit21::risk {

LossPreventionPolicy::LossPreventionPolicy()
    : platform::BusinessComponent(
          "LossPreventionPolicy",
          "fraud, loss and supplier risk controls",
          platform::BusinessComponentSpec{
              "risk",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult LossPreventionPolicy::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::risk
