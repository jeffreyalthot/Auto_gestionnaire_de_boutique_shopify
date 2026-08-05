#include "elit21/storage/Repository.h"

namespace elit21::storage {

Repository::Repository()
    : platform::BusinessComponent(
          "Repository",
          "SQLite persistence and transactional storage",
          platform::BusinessComponentSpec{
              "storage",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Repository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::storage
