#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class ProgressBar final : public platform::BusinessComponent {
public:
    ProgressBar();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
