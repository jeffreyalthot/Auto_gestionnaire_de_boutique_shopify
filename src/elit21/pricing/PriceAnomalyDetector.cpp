#include "elit21/pricing/PriceAnomalyDetector.h"

namespace elit21::pricing {

PriceAnomalyDetector::PriceAnomalyDetector()
    : platform::BusinessComponent(
          "PriceAnomalyDetector",
          "100 percent markup before shipping pricing enforcement",
          platform::BusinessComponentSpec{
              "pricing",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult PriceAnomalyDetector::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::pricing
