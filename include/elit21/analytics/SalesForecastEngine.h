#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::analytics {

class SalesForecastEngine final : public platform::BusinessComponent {
public:
    SalesForecastEngine();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::analytics
