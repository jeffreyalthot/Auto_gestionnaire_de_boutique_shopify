#include "elit21/catalog/ImagePolicyValidator.h"

namespace elit21::catalog {

ImagePolicyValidator::ImagePolicyValidator()
    : platform::BusinessComponent(
          "ImagePolicyValidator",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ImagePolicyValidator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
