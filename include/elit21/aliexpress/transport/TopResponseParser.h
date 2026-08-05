#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::aliexpress::transport {

class TopResponseParser final : public platform::BusinessComponent {
public:
    TopResponseParser();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::aliexpress::transport
