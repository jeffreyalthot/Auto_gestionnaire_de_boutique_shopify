#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::repositories {

class TaskRepository final : public platform::BusinessComponent {
public:
    TaskRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::repositories
