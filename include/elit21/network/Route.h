#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class Route final : public platform::BusinessComponent {
public:
    Route();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
