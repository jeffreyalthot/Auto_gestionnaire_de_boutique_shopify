#include "elit21/serialization/ModelSerializer.h"

namespace elit21::serialization {

ModelSerializer::ModelSerializer()
    : platform::BusinessComponent(
          "ModelSerializer",
          "JSON, CSV and request serialization",
          platform::BusinessComponentSpec{
              "serialization",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ModelSerializer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::serialization
