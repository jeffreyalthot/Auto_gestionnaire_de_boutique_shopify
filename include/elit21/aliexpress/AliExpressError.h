#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::aliexpress {

class AliExpressError final : public platform::BusinessComponent {
public:
    AliExpressError();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::aliexpress
