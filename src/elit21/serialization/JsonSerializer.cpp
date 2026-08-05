#include "elit21/serialization/JsonSerializer.h"

namespace elit21::serialization {

JsonSerializer::JsonSerializer()
    : platform::BusinessComponent(
          "JsonSerializer",
          "JSON, CSV and request serialization",
          platform::BusinessComponentSpec{
              "serialization",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult JsonSerializer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::serialization
