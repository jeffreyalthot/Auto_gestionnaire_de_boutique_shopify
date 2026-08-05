#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::application {

class DependencyContainer final : public platform::BusinessComponent {
public:
    DependencyContainer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::application
