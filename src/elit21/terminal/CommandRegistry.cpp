#include "elit21/terminal/CommandRegistry.h"

namespace elit21::terminal {

CommandRegistry::CommandRegistry()
    : platform::BusinessComponent(
          "CommandRegistry",
          "fixed-position terminal dashboard and command handling",
          platform::BusinessComponentSpec{
              "terminal",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CommandRegistry::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal
