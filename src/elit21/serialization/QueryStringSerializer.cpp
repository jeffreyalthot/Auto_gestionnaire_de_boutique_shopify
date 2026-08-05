#include "elit21/serialization/QueryStringSerializer.h"

namespace elit21::serialization {

QueryStringSerializer::QueryStringSerializer()
    : platform::BusinessComponent(
          "QueryStringSerializer",
          "JSON, CSV and request serialization",
          platform::BusinessComponentSpec{
              "serialization",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult QueryStringSerializer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::serialization
