#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::aliexpress::responses {

class TradeDsOrderGetResponse final : public platform::BusinessComponent {
public:
    TradeDsOrderGetResponse();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::aliexpress::responses
