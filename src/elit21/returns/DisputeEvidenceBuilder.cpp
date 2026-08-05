#include "elit21/returns/DisputeEvidenceBuilder.h"

namespace elit21::returns {

DisputeEvidenceBuilder::DisputeEvidenceBuilder()
    : platform::BusinessComponent(
          "DisputeEvidenceBuilder",
          "return, refund and dispute coordination",
          platform::BusinessComponentSpec{
              "returns",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DisputeEvidenceBuilder::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::returns
