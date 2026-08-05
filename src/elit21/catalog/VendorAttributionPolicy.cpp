#include "elit21/catalog/VendorAttributionPolicy.h"

namespace elit21::catalog {

VendorAttributionPolicy::VendorAttributionPolicy()
    : platform::BusinessComponent(
          "VendorAttributionPolicy",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult VendorAttributionPolicy::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
