#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::repositories {

class TrackingRepository final : public platform::BusinessComponent {
public:
    TrackingRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::repositories
