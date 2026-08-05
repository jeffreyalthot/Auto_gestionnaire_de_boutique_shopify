#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::messaging {

class MessageHeaders final : public platform::BusinessComponent {
public:
    MessageHeaders();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::messaging
