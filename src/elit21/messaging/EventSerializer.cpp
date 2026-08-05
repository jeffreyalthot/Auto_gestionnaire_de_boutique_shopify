#include "elit21/messaging/EventSerializer.h"

namespace elit21::messaging {

EventSerializer::EventSerializer()
    : platform::BusinessComponent(
          "EventSerializer",
          "command, query and event dispatch",
          platform::BusinessComponentSpec{
              "messaging",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult EventSerializer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::messaging
