#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::observability {

class HealthCheckRegistry final : public platform::BusinessComponent {
public:
    HealthCheckRegistry();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::observability
