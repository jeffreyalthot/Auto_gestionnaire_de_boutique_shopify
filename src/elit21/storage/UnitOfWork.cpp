#include "elit21/storage/UnitOfWork.h"

namespace elit21::storage {

UnitOfWork::UnitOfWork()
    : platform::BusinessComponent(
          "UnitOfWork",
          "SQLite persistence and transactional storage",
          platform::BusinessComponentSpec{
              "storage",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult UnitOfWork::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::storage
