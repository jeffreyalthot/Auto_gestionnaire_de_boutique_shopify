#include "elit21/messaging/QueryHandler.h"

namespace elit21::messaging {

QueryHandler::QueryHandler()
    : platform::BusinessComponent(
          "QueryHandler",
          "command, query and event dispatch",
          platform::BusinessComponentSpec{
              "messaging",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult QueryHandler::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::messaging
