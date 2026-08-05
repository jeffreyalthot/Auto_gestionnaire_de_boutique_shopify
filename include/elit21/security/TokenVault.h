#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::security {

class TokenVault final : public platform::BusinessComponent {
public:
    TokenVault();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::security
