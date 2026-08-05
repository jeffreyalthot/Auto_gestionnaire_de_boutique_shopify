#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::fulfillment {

class FulfillmentReconciliationService final : public platform::BusinessComponent {
public:
    FulfillmentReconciliationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::fulfillment
