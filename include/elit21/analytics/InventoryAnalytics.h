#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::analytics {

class InventoryAnalytics final : public platform::BusinessComponent {
public:
    InventoryAnalytics();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::analytics
