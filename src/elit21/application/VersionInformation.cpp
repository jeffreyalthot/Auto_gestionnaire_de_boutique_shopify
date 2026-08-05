#include "elit21/application/VersionInformation.h"

namespace elit21::application {

VersionInformation::VersionInformation()
    : platform::BusinessComponent(
          "VersionInformation",
          "application lifecycle and dependency orchestration",
          platform::BusinessComponentSpec{
              "application",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult VersionInformation::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::application
