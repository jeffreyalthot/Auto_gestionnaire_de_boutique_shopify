#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::analytics {

class OrderAnalytics final : public platform::BusinessComponent {
public:
    OrderAnalytics();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::analytics
