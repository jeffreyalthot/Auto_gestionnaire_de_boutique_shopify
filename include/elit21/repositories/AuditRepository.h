#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::repositories {

class AuditRepository final : public platform::BusinessComponent {
public:
    AuditRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::repositories
