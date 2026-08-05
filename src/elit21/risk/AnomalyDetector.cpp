#include "elit21/risk/AnomalyDetector.h"

namespace elit21::risk {

AnomalyDetector::AnomalyDetector()
    : platform::BusinessComponent(
          "AnomalyDetector",
          "fraud, loss and supplier risk controls",
          platform::BusinessComponentSpec{
              "risk",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AnomalyDetector::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::risk
