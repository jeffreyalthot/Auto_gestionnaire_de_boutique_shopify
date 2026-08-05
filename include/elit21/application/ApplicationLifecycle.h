#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::application {

class ApplicationLifecycle final : public platform::BusinessComponent {
public:
    ApplicationLifecycle();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::application
