#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::aliexpress::services {

class AliExpressCommissionService final : public platform::BusinessComponent {
public:
    AliExpressCommissionService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::aliexpress::services
