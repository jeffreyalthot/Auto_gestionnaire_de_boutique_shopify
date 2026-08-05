#include "elit21/terminal/panels/OrderPanel.h"

namespace elit21::terminal::panels {

OrderPanel::OrderPanel()
    : platform::BusinessComponent(
          "OrderPanel",
          "fixed-position terminal dashboard and command handling (panels)",
          platform::BusinessComponentSpec{
              "terminal",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderPanel::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal::panels
