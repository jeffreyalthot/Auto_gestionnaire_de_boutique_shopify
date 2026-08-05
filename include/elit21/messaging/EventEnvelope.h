#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::messaging {

class EventEnvelope final : public platform::BusinessComponent {
public:
    EventEnvelope();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::messaging
