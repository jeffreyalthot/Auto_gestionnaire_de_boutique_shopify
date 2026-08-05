#include "elit21/repositories/FinancialLedgerRepository.h"

namespace elit21::repositories {

FinancialLedgerRepository::FinancialLedgerRepository()
    : platform::BusinessComponent(
          "FinancialLedgerRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FinancialLedgerRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
