#include "elit21/messaging/EventEnvelope.h"

namespace elit21::messaging {

EventEnvelope::EventEnvelope()
    : platform::BusinessComponent(
          "EventEnvelope",
          "command, query and event dispatch",
          platform::BusinessComponentSpec{
              "messaging",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult EventEnvelope::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::messaging
