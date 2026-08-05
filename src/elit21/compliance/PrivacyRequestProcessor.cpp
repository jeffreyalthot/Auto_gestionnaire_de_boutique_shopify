#include "elit21/compliance/PrivacyRequestProcessor.h"

namespace elit21::compliance {

PrivacyRequestProcessor::PrivacyRequestProcessor()
    : platform::BusinessComponent(
          "PrivacyRequestProcessor",
          "Canadian product, privacy and address compliance",
          platform::BusinessComponentSpec{
              "compliance",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult PrivacyRequestProcessor::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::compliance
