#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::aliexpress {

class AliExpressConfig final : public platform::BusinessComponent {
public:
    AliExpressConfig();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::aliexpress
