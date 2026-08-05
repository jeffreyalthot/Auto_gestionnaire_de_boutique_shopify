#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::compliance {

class TaxJurisdictionResolver final : public platform::BusinessComponent {
public:
    TaxJurisdictionResolver();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::compliance
