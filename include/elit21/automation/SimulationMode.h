#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::automation {

class SimulationMode final : public platform::BusinessComponent {
public:
    SimulationMode();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::automation
