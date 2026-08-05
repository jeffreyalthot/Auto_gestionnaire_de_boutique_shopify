#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::resilience {

class TimeoutPolicy final : public platform::BusinessComponent {
public:
    TimeoutPolicy();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::resilience
