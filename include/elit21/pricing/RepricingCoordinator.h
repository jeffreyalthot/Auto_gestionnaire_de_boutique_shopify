#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class RepricingCoordinator final : public platform::BusinessComponent {
public:
    RepricingCoordinator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
