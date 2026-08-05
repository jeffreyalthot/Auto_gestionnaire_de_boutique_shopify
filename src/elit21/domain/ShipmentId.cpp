#include "elit21/domain/ShipmentId.h"

namespace elit21::domain {

ShipmentId::ShipmentId()
    : platform::BusinessComponent(
          "ShipmentId",
          "commerce domain value object",
          platform::BusinessComponentSpec{
              "domain",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ShipmentId::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::domain
