#include "elit21/compliance/ConsumerDisclosureValidator.h"

namespace elit21::compliance {

ConsumerDisclosureValidator::ConsumerDisclosureValidator()
    : platform::BusinessComponent(
          "ConsumerDisclosureValidator",
          "Canadian product, privacy and address compliance",
          platform::BusinessComponentSpec{
              "compliance",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ConsumerDisclosureValidator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::compliance
