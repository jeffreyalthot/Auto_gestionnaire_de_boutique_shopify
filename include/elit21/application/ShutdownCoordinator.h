#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::application {

class ShutdownCoordinator final : public platform::BusinessComponent {
public:
    ShutdownCoordinator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::application
