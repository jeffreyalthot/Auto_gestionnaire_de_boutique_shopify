#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::security {

class CredentialManager final : public platform::BusinessComponent {
public:
    CredentialManager();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::security
