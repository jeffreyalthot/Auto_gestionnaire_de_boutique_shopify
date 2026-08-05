#include "elit21/terminal/panels/FulfillmentPanel.h"

namespace elit21::terminal::panels {

FulfillmentPanel::FulfillmentPanel()
    : platform::BusinessComponent(
          "FulfillmentPanel",
          "fixed-position terminal dashboard and command handling (panels)",
          platform::BusinessComponentSpec{
              "terminal",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FulfillmentPanel::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal::panels
