#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::returns {

class AliExpressDisputeAdapter final : public platform::BusinessComponent {
public:
    AliExpressDisputeAdapter();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::returns
