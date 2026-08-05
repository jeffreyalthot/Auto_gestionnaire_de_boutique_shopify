#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::application {

class ServiceRegistry final : public platform::BusinessComponent {
public:
    ServiceRegistry();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::application
