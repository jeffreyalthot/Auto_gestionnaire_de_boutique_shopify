#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class OrderDeduplicationService final : public platform::BusinessComponent {
public:
    OrderDeduplicationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
