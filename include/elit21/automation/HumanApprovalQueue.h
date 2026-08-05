#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::automation {

class HumanApprovalQueue final : public platform::BusinessComponent {
public:
    HumanApprovalQueue();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::automation
