#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::repositories {

class IncidentRepository final : public platform::BusinessComponent {
public:
    IncidentRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::repositories
