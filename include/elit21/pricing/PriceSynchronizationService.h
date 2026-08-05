#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class PriceSynchronizationService final : public platform::BusinessComponent {
public:
    PriceSynchronizationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
