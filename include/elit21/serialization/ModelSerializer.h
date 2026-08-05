#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::serialization {

class ModelSerializer final : public platform::BusinessComponent {
public:
    ModelSerializer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::serialization
