#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::workflow {

class Saga final : public platform::BusinessComponent {
public:
    Saga();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::workflow
