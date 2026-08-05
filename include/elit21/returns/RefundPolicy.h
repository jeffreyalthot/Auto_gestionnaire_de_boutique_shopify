#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::returns {

class RefundPolicy final : public platform::BusinessComponent {
public:
    RefundPolicy();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::returns
