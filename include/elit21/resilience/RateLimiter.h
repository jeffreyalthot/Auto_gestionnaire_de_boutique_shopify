#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::resilience {

class RateLimiter final : public platform::BusinessComponent {
public:
    RateLimiter();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::resilience
