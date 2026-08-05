#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::aliexpress {

class AliExpressErrorMapper final : public platform::BusinessComponent {
public:
    AliExpressErrorMapper();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::aliexpress
