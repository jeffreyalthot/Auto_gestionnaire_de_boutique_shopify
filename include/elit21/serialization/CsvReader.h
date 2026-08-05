#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::serialization {

class CsvReader final : public platform::BusinessComponent {
public:
    CsvReader();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::serialization
