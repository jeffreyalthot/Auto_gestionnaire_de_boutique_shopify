#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::observability {

class HealthCheck final : public platform::BusinessComponent {
public:
    HealthCheck();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::observability
