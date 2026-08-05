#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal::panels {

class OrderPanel final : public platform::BusinessComponent {
public:
    OrderPanel();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal::panels
