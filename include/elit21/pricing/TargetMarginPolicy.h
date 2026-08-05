#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class TargetMarginPolicy final : public platform::BusinessComponent {
public:
    TargetMarginPolicy();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
