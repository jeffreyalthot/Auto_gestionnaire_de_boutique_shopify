#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class FreightSelectionPolicy final : public platform::BusinessComponent {
public:
    FreightSelectionPolicy();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
