#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::resilience {

class CircuitBreaker final : public platform::BusinessComponent {
public:
    CircuitBreaker();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::resilience
