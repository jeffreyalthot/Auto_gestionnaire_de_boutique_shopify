#include "elit21/storage/Cache.h"

namespace elit21::storage {

Cache::Cache()
    : platform::BusinessComponent(
          "Cache",
          "SQLite persistence and transactional storage",
          platform::BusinessComponentSpec{
              "storage",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Cache::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::storage
