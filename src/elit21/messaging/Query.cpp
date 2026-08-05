#include "elit21/messaging/Query.h"

namespace elit21::messaging {

Query::Query()
    : platform::BusinessComponent(
          "Query",
          "command, query and event dispatch",
          platform::BusinessComponentSpec{
              "messaging",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Query::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::messaging
