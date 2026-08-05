#include "elit21/orders/ShippingAddressSanitizer.h"

namespace elit21::orders {

ShippingAddressSanitizer::ShippingAddressSanitizer()
    : platform::BusinessComponent(
          "ShippingAddressSanitizer",
          "order validation and supplier placement workflow",
          platform::BusinessComponentSpec{
              "orders",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ShippingAddressSanitizer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::orders
