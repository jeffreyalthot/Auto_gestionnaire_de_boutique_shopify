#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::observability {

class FileLogSink final : public platform::BusinessComponent {
public:
    FileLogSink();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::observability
