#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class DutyEstimator final : public platform::BusinessComponent {
public:
    DutyEstimator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
