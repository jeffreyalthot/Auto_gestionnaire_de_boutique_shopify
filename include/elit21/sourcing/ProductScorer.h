#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::sourcing {

class ProductScorer final : public platform::BusinessComponent {
public:
    ProductScorer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::sourcing
