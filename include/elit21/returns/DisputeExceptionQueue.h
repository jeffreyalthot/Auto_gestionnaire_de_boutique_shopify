#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::returns {

class DisputeExceptionQueue final : public platform::BusinessComponent {
public:
    DisputeExceptionQueue();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::returns
