#include "elit21/mapping/CollectionMapper.h"

namespace elit21::mapping {

CollectionMapper::CollectionMapper()
    : platform::BusinessComponent(
          "CollectionMapper",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CollectionMapper::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
