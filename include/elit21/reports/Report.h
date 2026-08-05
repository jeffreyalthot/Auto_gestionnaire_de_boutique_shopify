#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::reports {

class Report final : public platform::BusinessComponent {
public:
    Report();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::reports
