#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::repositories {

class OutboxRepository final : public platform::BusinessComponent {
public:
    OutboxRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::repositories
