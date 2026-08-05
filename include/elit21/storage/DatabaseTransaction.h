#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::storage {

class DatabaseTransaction final : public platform::BusinessComponent {
public:
    DatabaseTransaction();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::storage
