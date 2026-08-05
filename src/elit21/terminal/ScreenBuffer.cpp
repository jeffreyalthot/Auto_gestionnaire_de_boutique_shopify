#include "elit21/terminal/ScreenBuffer.h"

namespace elit21::terminal {

ScreenBuffer::ScreenBuffer()
    : platform::BusinessComponent(
          "ScreenBuffer",
          "fixed-position terminal dashboard and command handling",
          platform::BusinessComponentSpec{
              "terminal",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ScreenBuffer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal
