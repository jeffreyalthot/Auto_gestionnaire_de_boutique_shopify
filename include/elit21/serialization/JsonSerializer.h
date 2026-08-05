#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::serialization {

class JsonSerializer final : public platform::BusinessComponent {
public:
    JsonSerializer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::serialization
