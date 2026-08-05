#include "elit21/storage/SqliteDatabase.h"

namespace elit21::storage {

SqliteDatabase::SqliteDatabase()
    : platform::BusinessComponent(
          "SqliteDatabase",
          "SQLite persistence and transactional storage",
          platform::BusinessComponentSpec{
              "storage",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SqliteDatabase::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::storage
