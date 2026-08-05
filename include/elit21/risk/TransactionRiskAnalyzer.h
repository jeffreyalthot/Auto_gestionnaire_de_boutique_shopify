#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::risk {

class TransactionRiskAnalyzer final : public platform::BusinessComponent {
public:
    TransactionRiskAnalyzer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::risk
