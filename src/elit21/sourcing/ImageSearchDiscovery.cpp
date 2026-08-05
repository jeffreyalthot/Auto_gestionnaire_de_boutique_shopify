#include "elit21/sourcing/ImageSearchDiscovery.h"

namespace elit21::sourcing {

ImageSearchDiscovery::ImageSearchDiscovery()
    : platform::BusinessComponent(
          "ImageSearchDiscovery",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ImageSearchDiscovery::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
