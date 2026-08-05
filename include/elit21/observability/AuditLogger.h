#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::observability {

class AuditLogger final : public platform::BusinessComponent {
public:
    AuditLogger();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::observability
