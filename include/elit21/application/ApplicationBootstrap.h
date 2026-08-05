#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::application {

class ApplicationBootstrap final : public platform::BusinessComponent {
public:
    ApplicationBootstrap();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::application
