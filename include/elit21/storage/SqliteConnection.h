#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::storage {

class SqliteConnection final : public platform::BusinessComponent {
public:
    SqliteConnection();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::storage
