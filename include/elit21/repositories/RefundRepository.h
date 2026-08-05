#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::repositories {

class RefundRepository final : public platform::BusinessComponent {
public:
    RefundRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::repositories
