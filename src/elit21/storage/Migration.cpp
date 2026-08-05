#include "elit21/storage/Migration.h"

namespace elit21::storage {

Migration::Migration()
    : platform::BusinessComponent(
          "Migration",
          "SQLite persistence and transactional storage",
          platform::BusinessComponentSpec{
              "storage",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Migration::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::storage
