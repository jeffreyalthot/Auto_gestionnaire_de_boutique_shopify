#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::resilience {

class RetryPolicy final : public platform::BusinessComponent {
public:
    RetryPolicy();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::resilience
