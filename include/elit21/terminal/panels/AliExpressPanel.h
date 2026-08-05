#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal::panels {

class AliExpressPanel final : public platform::BusinessComponent {
public:
    AliExpressPanel();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal::panels
