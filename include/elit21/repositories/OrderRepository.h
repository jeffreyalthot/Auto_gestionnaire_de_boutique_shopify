#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::repositories {

class OrderRepository final : public platform::BusinessComponent {
public:
    OrderRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::repositories
