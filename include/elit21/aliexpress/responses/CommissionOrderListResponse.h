#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::aliexpress::responses {

class CommissionOrderListResponse final : public platform::BusinessComponent {
public:
    CommissionOrderListResponse();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::aliexpress::responses
