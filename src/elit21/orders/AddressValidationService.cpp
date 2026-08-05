#include "elit21/orders/AddressValidationService.h"

namespace elit21::orders {

AddressValidationService::AddressValidationService()
    : platform::BusinessComponent(
          "AddressValidationService",
          "order validation and supplier placement workflow",
          platform::BusinessComponentSpec{
              "orders",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AddressValidationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::orders
