#include "elit21/storage/Row.h"

namespace elit21::storage {

Row::Row()
    : platform::BusinessComponent(
          "Row",
          "SQLite persistence and transactional storage",
          platform::BusinessComponentSpec{
              "storage",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Row::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::storage
