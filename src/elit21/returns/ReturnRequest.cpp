#include "elit21/returns/ReturnRequest.h"

namespace elit21::returns {

ReturnRequest::ReturnRequest()
    : platform::BusinessComponent(
          "ReturnRequest",
          "return, refund and dispute coordination",
          platform::BusinessComponentSpec{
              "returns",
              "command",
              std::vector<std::string>{"order_id"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ReturnRequest::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::returns
