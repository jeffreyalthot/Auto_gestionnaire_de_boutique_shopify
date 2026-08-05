#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::observability {

class CrashReporter final : public platform::BusinessComponent {
public:
    CrashReporter();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::observability
