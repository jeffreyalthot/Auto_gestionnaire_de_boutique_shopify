#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class TaxEstimator final : public platform::BusinessComponent {
public:
    TaxEstimator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
