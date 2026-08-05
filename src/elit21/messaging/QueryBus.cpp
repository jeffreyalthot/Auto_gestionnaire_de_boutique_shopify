#include "elit21/messaging/QueryBus.h"

namespace elit21::messaging {

QueryBus::QueryBus()
    : platform::BusinessComponent(
          "QueryBus",
          "command, query and event dispatch",
          platform::BusinessComponentSpec{
              "messaging",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult QueryBus::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::messaging
