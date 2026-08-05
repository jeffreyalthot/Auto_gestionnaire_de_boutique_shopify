#include "elit21/storage/Entity.h"

namespace elit21::storage {

Entity::Entity()
    : platform::BusinessComponent(
          "Entity",
          "SQLite persistence and transactional storage",
          platform::BusinessComponentSpec{
              "storage",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Entity::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::storage
