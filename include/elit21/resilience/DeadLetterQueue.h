#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::resilience {

class DeadLetterQueue final : public platform::BusinessComponent {
public:
    DeadLetterQueue();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::resilience
