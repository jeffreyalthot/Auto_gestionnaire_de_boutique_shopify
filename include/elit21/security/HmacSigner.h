#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::security {

class HmacSigner final : public platform::BusinessComponent {
public:
    HmacSigner();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::security
