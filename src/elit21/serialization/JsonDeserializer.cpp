#include "elit21/serialization/JsonDeserializer.h"

namespace elit21::serialization {

JsonDeserializer::JsonDeserializer()
    : platform::BusinessComponent(
          "JsonDeserializer",
          "JSON, CSV and request serialization",
          platform::BusinessComponentSpec{
              "serialization",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult JsonDeserializer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::serialization
