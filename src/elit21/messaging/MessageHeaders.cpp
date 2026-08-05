#include "elit21/messaging/MessageHeaders.h"

namespace elit21::messaging {

MessageHeaders::MessageHeaders()
    : platform::BusinessComponent(
          "MessageHeaders",
          "command, query and event dispatch",
          platform::BusinessComponentSpec{
              "messaging",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult MessageHeaders::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::messaging
