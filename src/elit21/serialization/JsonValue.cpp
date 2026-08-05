#include "elit21/serialization/JsonValue.h"

namespace elit21::serialization {

JsonValue::JsonValue()
    : platform::BusinessComponent(
          "JsonValue",
          "JSON, CSV and request serialization",
          platform::BusinessComponentSpec{
              "serialization",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult JsonValue::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::serialization
