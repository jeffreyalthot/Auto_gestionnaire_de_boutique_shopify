#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::serialization {

class JsonValue final : public platform::BusinessComponent {
public:
    JsonValue();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::serialization
