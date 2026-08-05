#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::catalog {

class VendorAttributionPolicy final : public platform::BusinessComponent {
public:
    VendorAttributionPolicy();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::catalog
