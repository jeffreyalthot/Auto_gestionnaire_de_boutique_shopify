#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::aliexpress::transport {

class AliExpressRateLimiter final : public platform::BusinessComponent {
public:
    AliExpressRateLimiter();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::aliexpress::transport
