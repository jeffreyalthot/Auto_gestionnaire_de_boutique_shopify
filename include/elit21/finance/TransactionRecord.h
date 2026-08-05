#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::finance {

class TransactionRecord final : public platform::BusinessComponent {
public:
    TransactionRecord();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::finance
