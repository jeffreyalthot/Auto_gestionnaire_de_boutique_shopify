#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::analytics {

class FulfillmentAnalytics final : public platform::BusinessComponent {
public:
    FulfillmentAnalytics();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::analytics
