#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::reports {

class ReportExporter final : public platform::BusinessComponent {
public:
    ReportExporter();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::reports
