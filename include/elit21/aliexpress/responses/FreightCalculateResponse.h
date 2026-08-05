#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::aliexpress::responses {

class FreightCalculateResponse final : public platform::BusinessComponent {
public:
    FreightCalculateResponse();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::aliexpress::responses
