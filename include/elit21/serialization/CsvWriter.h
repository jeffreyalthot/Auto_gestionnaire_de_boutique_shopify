#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::serialization {

class CsvWriter final : public platform::BusinessComponent {
public:
    CsvWriter();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::serialization
