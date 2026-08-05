#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class PartialOrderCoordinator final : public platform::BusinessComponent {
public:
    PartialOrderCoordinator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
