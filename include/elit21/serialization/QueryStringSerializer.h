#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::serialization {

class QueryStringSerializer final : public platform::BusinessComponent {
public:
    QueryStringSerializer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::serialization
