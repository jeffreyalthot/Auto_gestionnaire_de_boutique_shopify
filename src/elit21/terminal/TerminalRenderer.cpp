#include "elit21/terminal/TerminalRenderer.h"

namespace elit21::terminal {

TerminalRenderer::TerminalRenderer()
    : platform::BusinessComponent(
          "TerminalRenderer",
          "fixed-position terminal dashboard and command handling",
          platform::BusinessComponentSpec{
              "terminal",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TerminalRenderer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal
