#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::aliexpress {

class AliExpressServiceFactory final : public platform::BusinessComponent {
public:
    AliExpressServiceFactory();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::aliexpress
