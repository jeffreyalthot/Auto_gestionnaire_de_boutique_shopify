#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::storage {

class QueryBuilder final : public platform::BusinessComponent {
public:
    QueryBuilder();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::storage
