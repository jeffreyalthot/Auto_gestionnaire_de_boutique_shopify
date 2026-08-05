#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::observability {

class Tracer final : public platform::BusinessComponent {
public:
    Tracer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::observability
