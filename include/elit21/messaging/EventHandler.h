#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::messaging {

class EventHandler final : public platform::BusinessComponent {
public:
    EventHandler();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::messaging
