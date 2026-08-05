#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class Layout final : public platform::BusinessComponent {
public:
    Layout();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
