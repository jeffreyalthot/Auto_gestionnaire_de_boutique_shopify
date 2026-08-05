#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class UnicodeSanitizer final : public platform::BusinessComponent {
public:
    UnicodeSanitizer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
