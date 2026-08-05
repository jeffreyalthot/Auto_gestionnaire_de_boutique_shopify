#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class CommandLine final : public platform::BusinessComponent {
public:
    CommandLine();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
