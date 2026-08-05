#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal::commands {

class CatalogCommand final : public platform::BusinessComponent {
public:
    CatalogCommand();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal::commands
