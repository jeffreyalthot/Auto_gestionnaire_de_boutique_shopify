#include "elit21/terminal/panels/HealthPanel.h"

namespace elit21::terminal::panels {

HealthPanel::HealthPanel()
    : platform::BusinessComponent(
          "HealthPanel",
          "fixed-position terminal dashboard and command handling (panels)",
          platform::BusinessComponentSpec{
              "terminal",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult HealthPanel::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal::panels
