#include "elit21/mapping/InventoryMapper.h"

namespace elit21::mapping {

InventoryMapper::InventoryMapper()
    : platform::BusinessComponent(
          "InventoryMapper",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "validate_transform",
              std::vector<std::string>{"sku"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult InventoryMapper::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
