#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::resilience {

class BackoffPolicy final : public platform::BusinessComponent {
public:
    BackoffPolicy();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::resilience
