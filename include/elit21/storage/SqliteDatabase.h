#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::storage {

class SqliteDatabase final : public platform::BusinessComponent {
public:
    SqliteDatabase();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::storage
