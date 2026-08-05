#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::sourcing {

class CompetitionAnalyzer final : public platform::BusinessComponent {
public:
    CompetitionAnalyzer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::sourcing
