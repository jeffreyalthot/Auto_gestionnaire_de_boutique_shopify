#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::observability {

class StructuredLogger final : public platform::BusinessComponent {
public:
    StructuredLogger();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::observability
