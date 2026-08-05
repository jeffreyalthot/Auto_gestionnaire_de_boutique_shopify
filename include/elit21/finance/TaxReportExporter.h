#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::finance {

class TaxReportExporter final : public platform::BusinessComponent {
public:
    TaxReportExporter();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::finance
