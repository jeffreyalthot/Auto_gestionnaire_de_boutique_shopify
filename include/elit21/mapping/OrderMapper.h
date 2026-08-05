#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::mapping {

class OrderMapper final : public platform::BusinessComponent {
public:
    OrderMapper();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::mapping
