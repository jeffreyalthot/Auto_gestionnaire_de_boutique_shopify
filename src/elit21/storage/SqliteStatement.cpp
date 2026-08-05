#include "elit21/storage/SqliteStatement.h"

namespace elit21::storage {

SqliteStatement::SqliteStatement()
    : platform::BusinessComponent(
          "SqliteStatement",
          "SQLite persistence and transactional storage",
          platform::BusinessComponentSpec{
              "storage",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SqliteStatement::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::storage
