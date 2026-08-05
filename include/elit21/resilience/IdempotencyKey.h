#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::resilience {

class IdempotencyKey final : public platform::BusinessComponent {
public:
    IdempotencyKey();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::resilience
