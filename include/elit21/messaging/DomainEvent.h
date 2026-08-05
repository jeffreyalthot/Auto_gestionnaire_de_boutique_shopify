#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::messaging {

class DomainEvent final : public platform::BusinessComponent {
public:
    DomainEvent();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::messaging
