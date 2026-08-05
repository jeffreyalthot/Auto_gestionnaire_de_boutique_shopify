#include "elit21/catalog/ProductTagGenerator.h"

namespace elit21::catalog {

ProductTagGenerator::ProductTagGenerator()
    : platform::BusinessComponent(
          "ProductTagGenerator",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductTagGenerator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
