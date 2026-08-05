#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::security {

class Secret final : public platform::BusinessComponent {
public:
    Secret();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::security
