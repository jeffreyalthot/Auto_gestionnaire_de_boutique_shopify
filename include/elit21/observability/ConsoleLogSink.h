#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::observability {

class ConsoleLogSink final : public platform::BusinessComponent {
public:
    ConsoleLogSink();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::observability
