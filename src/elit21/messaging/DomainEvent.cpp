#include "elit21/messaging/DomainEvent.h"

namespace elit21::messaging {

DomainEvent::DomainEvent()
    : platform::BusinessComponent(
          "DomainEvent",
          "command, query and event dispatch",
          platform::BusinessComponentSpec{
              "messaging",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DomainEvent::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::messaging
