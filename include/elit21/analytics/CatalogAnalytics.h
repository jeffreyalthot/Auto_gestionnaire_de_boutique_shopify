#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::analytics {

class CatalogAnalytics final : public platform::BusinessComponent {
public:
    CatalogAnalytics();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::analytics
