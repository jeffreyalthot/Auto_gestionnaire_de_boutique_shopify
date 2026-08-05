#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::returns {

class RefundCoordinator final : public platform::BusinessComponent {
public:
    RefundCoordinator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::returns
