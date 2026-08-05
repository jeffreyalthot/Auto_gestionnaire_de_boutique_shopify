#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class TableRenderer final : public platform::BusinessComponent {
public:
    TableRenderer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
