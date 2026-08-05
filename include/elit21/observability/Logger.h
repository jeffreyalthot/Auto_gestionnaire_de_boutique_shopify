#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::observability {

class Logger final : public platform::BusinessComponent {
public:
    Logger();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::observability
