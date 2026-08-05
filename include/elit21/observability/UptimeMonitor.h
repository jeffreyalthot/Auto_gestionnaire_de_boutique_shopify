#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::observability {

class UptimeMonitor final : public platform::BusinessComponent {
public:
    UptimeMonitor();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::observability
