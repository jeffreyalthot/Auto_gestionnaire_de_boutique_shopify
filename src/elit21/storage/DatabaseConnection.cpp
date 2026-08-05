#include "elit21/storage/DatabaseConnection.h"

namespace elit21::storage {

DatabaseConnection::DatabaseConnection()
    : platform::BusinessComponent(
          "DatabaseConnection",
          "SQLite persistence and transactional storage",
          platform::BusinessComponentSpec{
              "storage",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DatabaseConnection::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::storage
