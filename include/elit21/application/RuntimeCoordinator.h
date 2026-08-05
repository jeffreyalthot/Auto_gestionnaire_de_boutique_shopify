#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::application {

class RuntimeCoordinator final : public platform::BusinessComponent {
public:
    RuntimeCoordinator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::application
