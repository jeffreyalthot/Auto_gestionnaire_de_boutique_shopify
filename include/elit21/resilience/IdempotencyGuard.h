#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::resilience {

class IdempotencyGuard final : public platform::BusinessComponent {
public:
    IdempotencyGuard();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::resilience
