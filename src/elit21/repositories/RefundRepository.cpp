#include "elit21/repositories/RefundRepository.h"

namespace elit21::repositories {

RefundRepository::RefundRepository()
    : platform::BusinessComponent(
          "RefundRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "command",
              std::vector<std::string>{"order_id"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RefundRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
