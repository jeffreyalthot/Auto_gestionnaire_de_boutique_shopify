#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::messaging {

class EventBus final : public platform::BusinessComponent {
public:
    EventBus();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::messaging
