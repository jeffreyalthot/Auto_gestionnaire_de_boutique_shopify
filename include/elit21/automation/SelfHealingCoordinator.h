#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::automation {

class SelfHealingCoordinator final : public platform::BusinessComponent {
public:
    SelfHealingCoordinator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::automation
