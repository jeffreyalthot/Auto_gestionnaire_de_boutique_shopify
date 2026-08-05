#include "elit21/compliance/ProvinceResolver.h"

namespace elit21::compliance {

ProvinceResolver::ProvinceResolver()
    : platform::BusinessComponent(
          "ProvinceResolver",
          "Canadian product, privacy and address compliance",
          platform::BusinessComponentSpec{
              "compliance",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProvinceResolver::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::compliance
