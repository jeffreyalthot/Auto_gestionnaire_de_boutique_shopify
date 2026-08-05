#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class OrderPlacementSaga final : public platform::BusinessComponent {
public:
    OrderPlacementSaga();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
