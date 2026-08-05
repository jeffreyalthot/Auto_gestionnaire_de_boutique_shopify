#include "elit21/storage/CacheEntry.h"

namespace elit21::storage {

CacheEntry::CacheEntry()
    : platform::BusinessComponent(
          "CacheEntry",
          "SQLite persistence and transactional storage",
          platform::BusinessComponentSpec{
              "storage",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CacheEntry::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::storage
