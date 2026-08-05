#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::application {

class RuntimeState final : public platform::BusinessComponent {
public:
    RuntimeState();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::application
