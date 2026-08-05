#include "elit21/storage/BackupManager.h"

namespace elit21::storage {

BackupManager::BackupManager()
    : platform::BusinessComponent(
          "BackupManager",
          "SQLite persistence and transactional storage",
          platform::BusinessComponentSpec{
              "storage",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult BackupManager::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::storage
