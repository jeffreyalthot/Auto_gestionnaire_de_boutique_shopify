#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::application {

class ApplicationContext final : public platform::BusinessComponent {
public:
    ApplicationContext();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::application
