#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::customer_service {

class BilingualMessageBuilder final : public platform::BusinessComponent {
public:
    BilingualMessageBuilder();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::customer_service
