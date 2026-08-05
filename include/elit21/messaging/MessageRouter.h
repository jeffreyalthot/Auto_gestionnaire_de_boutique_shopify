#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::messaging {

class MessageRouter final : public platform::BusinessComponent {
public:
    MessageRouter();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::messaging
