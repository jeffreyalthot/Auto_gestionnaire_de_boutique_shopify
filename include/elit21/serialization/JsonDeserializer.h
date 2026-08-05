#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::serialization {

class JsonDeserializer final : public platform::BusinessComponent {
public:
    JsonDeserializer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::serialization
