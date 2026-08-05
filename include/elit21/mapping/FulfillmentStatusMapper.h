#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::mapping {

class FulfillmentStatusMapper final : public platform::BusinessComponent {
public:
    FulfillmentStatusMapper();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::mapping
