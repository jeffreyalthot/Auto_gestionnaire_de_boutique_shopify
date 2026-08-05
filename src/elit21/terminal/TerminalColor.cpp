#include "elit21/terminal/TerminalColor.h"

namespace elit21::terminal {

TerminalColor::TerminalColor()
    : platform::BusinessComponent(
          "TerminalColor",
          "fixed-position terminal dashboard and command handling",
          platform::BusinessComponentSpec{
              "terminal",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TerminalColor::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal
