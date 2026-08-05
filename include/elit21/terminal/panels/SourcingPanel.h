#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal::panels {

class SourcingPanel final : public platform::BusinessComponent {
public:
    SourcingPanel();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal::panels
