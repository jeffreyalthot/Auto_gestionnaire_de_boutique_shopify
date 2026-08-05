#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::sourcing {

class CandidateProductRepository final : public platform::BusinessComponent {
public:
    CandidateProductRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::sourcing
