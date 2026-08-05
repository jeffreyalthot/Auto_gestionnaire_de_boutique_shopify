#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::observability {

class TraceContext final : public platform::BusinessComponent {
public:
    TraceContext();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::observability
