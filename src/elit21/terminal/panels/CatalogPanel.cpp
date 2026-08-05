#include "elit21/terminal/panels/CatalogPanel.h"

namespace elit21::terminal::panels {

CatalogPanel::CatalogPanel()
    : platform::BusinessComponent(
          "CatalogPanel",
          "fixed-position terminal dashboard and command handling (panels)",
          platform::BusinessComponentSpec{
              "terminal",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CatalogPanel::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal::panels
