#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::configuration {

class FulfillmentConfig final : public platform::BusinessComponent {
public:
    FulfillmentConfig();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::configuration
