#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::configuration {

class DatabaseConfig final : public platform::BusinessComponent {
public:
    DatabaseConfig();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::configuration
