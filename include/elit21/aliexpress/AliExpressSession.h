#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::aliexpress {

class AliExpressSession final : public platform::BusinessComponent {
public:
    AliExpressSession();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::aliexpress
