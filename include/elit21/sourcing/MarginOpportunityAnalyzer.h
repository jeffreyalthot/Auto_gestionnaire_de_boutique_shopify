#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::sourcing {

class MarginOpportunityAnalyzer final : public platform::BusinessComponent {
public:
    MarginOpportunityAnalyzer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::sourcing
