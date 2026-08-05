#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal::panels {

class SystemPanel final : public platform::BusinessComponent {
public:
    SystemPanel();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal::panels
