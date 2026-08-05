#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::messaging {

class EventSerializer final : public platform::BusinessComponent {
public:
    EventSerializer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::messaging
