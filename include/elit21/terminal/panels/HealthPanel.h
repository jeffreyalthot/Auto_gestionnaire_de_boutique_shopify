#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal::panels {

class HealthPanel final : public platform::BusinessComponent {
public:
    HealthPanel();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal::panels
