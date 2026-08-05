#include "elit21/storage/SqliteConnection.h"

namespace elit21::storage {

SqliteConnection::SqliteConnection()
    : platform::BusinessComponent(
          "SqliteConnection",
          "SQLite persistence and transactional storage",
          platform::BusinessComponentSpec{
              "storage",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SqliteConnection::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::storage
