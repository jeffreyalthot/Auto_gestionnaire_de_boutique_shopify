#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::aliexpress::transport {

class AliExpressApiTransport final : public platform::BusinessComponent {
public:
    AliExpressApiTransport();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::aliexpress::transport
