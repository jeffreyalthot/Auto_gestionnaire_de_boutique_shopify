#include "elit21/terminal/TerminalApplication.h"

namespace elit21::terminal {

TerminalApplication::TerminalApplication()
    : platform::BusinessComponent(
          "TerminalApplication",
          "fixed-position terminal dashboard and command handling",
          platform::BusinessComponentSpec{
              "terminal",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TerminalApplication::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal
