#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::repositories {

class MetricRepository final : public platform::BusinessComponent {
public:
    MetricRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::repositories
