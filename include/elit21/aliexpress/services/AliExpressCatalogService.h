#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::aliexpress::services {

class AliExpressCatalogService final : public platform::BusinessComponent {
public:
    AliExpressCatalogService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::aliexpress::services
