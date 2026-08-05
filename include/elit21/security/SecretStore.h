#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::security {

class SecretStore final : public platform::BusinessComponent {
public:
    SecretStore();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::security
