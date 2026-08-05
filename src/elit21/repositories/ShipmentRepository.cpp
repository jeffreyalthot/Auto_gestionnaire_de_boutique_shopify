#include "elit21/repositories/ShipmentRepository.h"

namespace elit21::repositories {

ShipmentRepository::ShipmentRepository()
    : platform::BusinessComponent(
          "ShipmentRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ShipmentRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
