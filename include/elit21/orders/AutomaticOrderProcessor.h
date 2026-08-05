#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class AutomaticOrderProcessor final : public platform::BusinessComponent {
public:
    AutomaticOrderProcessor();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
