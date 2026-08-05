#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::application {

class Application final : public platform::BusinessComponent {
public:
    Application();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::application
