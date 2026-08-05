#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::repositories {

class DeadLetterRepository final : public platform::BusinessComponent {
public:
    DeadLetterRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::repositories
