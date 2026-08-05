#include "elit21/storage/MigrationRepository.h"

namespace elit21::storage {

MigrationRepository::MigrationRepository()
    : platform::BusinessComponent(
          "MigrationRepository",
          "SQLite persistence and transactional storage",
          platform::BusinessComponentSpec{
              "storage",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult MigrationRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::storage
