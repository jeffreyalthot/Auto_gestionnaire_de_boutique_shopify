#include "elit21/storage/StorageHealthCheck.h"

namespace elit21::storage {

StorageHealthCheck::StorageHealthCheck()
    : platform::BusinessComponent(
          "StorageHealthCheck",
          "SQLite persistence and transactional storage",
          platform::BusinessComponentSpec{
              "storage",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult StorageHealthCheck::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::storage
