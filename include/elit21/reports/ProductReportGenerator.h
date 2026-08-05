#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::reports {

class ProductReportGenerator final : public platform::BusinessComponent {
public:
    ProductReportGenerator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::reports
