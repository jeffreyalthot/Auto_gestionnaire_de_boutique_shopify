#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::sourcing {

class SellerScorer final : public platform::BusinessComponent {
public:
    SellerScorer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::sourcing
