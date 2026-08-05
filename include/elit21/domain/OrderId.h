#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::domain {

class OrderId final : public platform::BusinessComponent {
public:
    OrderId();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::domain
