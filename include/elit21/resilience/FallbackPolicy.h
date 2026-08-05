#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::resilience {

class FallbackPolicy final : public platform::BusinessComponent {
public:
    FallbackPolicy();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::resilience
