#include "elit21/storage/DatabaseTransaction.h"

namespace elit21::storage {

DatabaseTransaction::DatabaseTransaction()
    : platform::BusinessComponent(
          "DatabaseTransaction",
          "SQLite persistence and transactional storage",
          platform::BusinessComponentSpec{
              "storage",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DatabaseTransaction::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::storage
