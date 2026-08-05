#include "elit21/storage/QueryBuilder.h"

namespace elit21::storage {

QueryBuilder::QueryBuilder()
    : platform::BusinessComponent(
          "QueryBuilder",
          "SQLite persistence and transactional storage",
          platform::BusinessComponentSpec{
              "storage",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult QueryBuilder::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::storage
