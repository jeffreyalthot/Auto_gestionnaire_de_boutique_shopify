#include "elit21/mapping/TrackingStatusMapper.h"

namespace elit21::mapping {

TrackingStatusMapper::TrackingStatusMapper()
    : platform::BusinessComponent(
          "TrackingStatusMapper",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "validate_transform",
              std::vector<std::string>{"tracking_number"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TrackingStatusMapper::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
