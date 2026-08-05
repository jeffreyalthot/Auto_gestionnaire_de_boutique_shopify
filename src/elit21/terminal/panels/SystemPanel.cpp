#include "elit21/terminal/panels/SystemPanel.h"

namespace elit21::terminal::panels {

SystemPanel::SystemPanel()
    : platform::BusinessComponent(
          "SystemPanel",
          "fixed-position terminal dashboard and command handling (panels)",
          platform::BusinessComponentSpec{
              "terminal",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SystemPanel::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal::panels
