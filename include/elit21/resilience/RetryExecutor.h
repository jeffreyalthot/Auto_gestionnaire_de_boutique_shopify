#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::resilience {

class RetryExecutor final : public platform::BusinessComponent {
public:
    RetryExecutor();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::resilience
