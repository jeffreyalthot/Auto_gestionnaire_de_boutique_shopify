#include "elit21/compliance/DataRetentionPolicy.h"

namespace elit21::compliance {

DataRetentionPolicy::DataRetentionPolicy()
    : platform::BusinessComponent(
          "DataRetentionPolicy",
          "Canadian product, privacy and address compliance",
          platform::BusinessComponentSpec{
              "compliance",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DataRetentionPolicy::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::compliance
