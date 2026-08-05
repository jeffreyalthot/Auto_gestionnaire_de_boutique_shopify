#include "elit21/storage/DatabaseMigrator.h"

namespace elit21::storage {

DatabaseMigrator::DatabaseMigrator()
    : platform::BusinessComponent(
          "DatabaseMigrator",
          "SQLite persistence and transactional storage",
          platform::BusinessComponentSpec{
              "storage",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DatabaseMigrator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::storage
