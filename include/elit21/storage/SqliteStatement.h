#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::storage {

class SqliteStatement final : public platform::BusinessComponent {
public:
    SqliteStatement();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::storage
