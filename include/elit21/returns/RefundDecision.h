#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::returns {

class RefundDecision final : public platform::BusinessComponent {
public:
    RefundDecision();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::returns
