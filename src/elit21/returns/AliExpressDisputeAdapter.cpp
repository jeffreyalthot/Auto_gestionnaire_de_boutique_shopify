#include "elit21/returns/AliExpressDisputeAdapter.h"

namespace elit21::returns {

AliExpressDisputeAdapter::AliExpressDisputeAdapter()
    : platform::BusinessComponent(
          "AliExpressDisputeAdapter",
          "return, refund and dispute coordination",
          platform::BusinessComponentSpec{
              "returns",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressDisputeAdapter::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::returns
