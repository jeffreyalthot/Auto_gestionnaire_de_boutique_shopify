#include "elit21/terminal/TerminalEvent.h"

namespace elit21::terminal {

TerminalEvent::TerminalEvent()
    : platform::BusinessComponent(
          "TerminalEvent",
          "fixed-position terminal dashboard and command handling",
          platform::BusinessComponentSpec{
              "terminal",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TerminalEvent::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal
