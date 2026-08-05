#include "elit21/domain/TaxRate.h"

namespace elit21::domain {

TaxRate::TaxRate()
    : platform::BusinessComponent(
          "TaxRate",
          "commerce domain value object",
          platform::BusinessComponentSpec{
              "domain",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TaxRate::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::domain
