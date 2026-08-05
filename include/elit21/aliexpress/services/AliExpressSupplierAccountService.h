#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::aliexpress::services {

class AliExpressSupplierAccountService final : public platform::BusinessComponent {
public:
    AliExpressSupplierAccountService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::aliexpress::services
