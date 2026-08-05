#include "elit21/repositories/ReturnRepository.h"

namespace elit21::repositories {

ReturnRepository::ReturnRepository()
    : platform::BusinessComponent(
          "ReturnRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "command",
              std::vector<std::string>{"order_id"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ReturnRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
