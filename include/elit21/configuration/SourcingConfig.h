#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::configuration {

class SourcingConfig final : public platform::BusinessComponent {
public:
    SourcingConfig();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::configuration
