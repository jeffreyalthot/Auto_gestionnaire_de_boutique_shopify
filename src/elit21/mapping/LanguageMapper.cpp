#include "elit21/mapping/LanguageMapper.h"

namespace elit21::mapping {

LanguageMapper::LanguageMapper()
    : platform::BusinessComponent(
          "LanguageMapper",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult LanguageMapper::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
