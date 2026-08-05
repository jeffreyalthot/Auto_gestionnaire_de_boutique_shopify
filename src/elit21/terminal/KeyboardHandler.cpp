#include "elit21/terminal/KeyboardHandler.h"

namespace elit21::terminal {

KeyboardHandler::KeyboardHandler()
    : platform::BusinessComponent(
          "KeyboardHandler",
          "fixed-position terminal dashboard and command handling",
          platform::BusinessComponentSpec{
              "terminal",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult KeyboardHandler::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal
