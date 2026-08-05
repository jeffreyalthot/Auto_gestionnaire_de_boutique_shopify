#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::analytics {

class ConversionAnalytics final : public platform::BusinessComponent {
public:
    ConversionAnalytics();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::analytics
