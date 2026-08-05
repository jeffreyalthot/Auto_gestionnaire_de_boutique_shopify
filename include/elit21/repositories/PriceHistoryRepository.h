#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::repositories {

class PriceHistoryRepository final : public platform::BusinessComponent {
public:
    PriceHistoryRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::repositories
