#include "elit21/application/StartupSequence.h"

namespace elit21::application {

StartupSequence::StartupSequence()
    : platform::BusinessComponent(
          "StartupSequence",
          "application lifecycle and dependency orchestration",
          platform::BusinessComponentSpec{
              "application",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult StartupSequence::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::application
