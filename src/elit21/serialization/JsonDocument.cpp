#include "elit21/serialization/JsonDocument.h"

namespace elit21::serialization {

JsonDocument::JsonDocument()
    : platform::BusinessComponent(
          "JsonDocument",
          "JSON, CSV and request serialization",
          platform::BusinessComponentSpec{
              "serialization",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult JsonDocument::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::serialization
