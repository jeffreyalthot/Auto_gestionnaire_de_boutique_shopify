#include "elit21/inventory/RestockDetector.h"

namespace elit21::inventory {

RestockDetector::RestockDetector()
    : platform::BusinessComponent(
          "RestockDetector",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RestockDetector::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
