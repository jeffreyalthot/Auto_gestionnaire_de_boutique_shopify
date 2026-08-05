#pragma once

#include "elit21/json/Json.h"
#include "elit21/platform/BusinessComponent.h"

#include <cstdint>
#include <string>
#include <vector>

namespace elit21::terminal::panels {

struct ShopifyPanelSnapshot {
    bool connected{false};
    bool dry_run{true};
    int readiness_score{0};
    std::uint64_t graphql_requests{0};
    std::uint64_t graphql_retries{0};
    std::uint64_t graphql_throttles{0};
    std::uint64_t webhooks_accepted{0};
    std::uint64_t webhooks_rejected{0};
    std::uint64_t pending_tasks{0};
    std::uint64_t manual_review_orders{0};
    double webhook_p90_response_ms{0.0};
    double webhook_failure_rate_percent{0.0};
    int api_contract_score{0};
    std::string circuit_state{"closed"};
    std::string api_version;
    std::string last_event;
    [[nodiscard]] Json toJson() const;
    [[nodiscard]] std::vector<std::string> renderLines(std::size_t width = 78) const;
};

class ShopifyPanel final : public platform::BusinessComponent {
public:
    ShopifyPanel();
    static ShopifyPanelSnapshot fromJson(const Json& value);
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal::panels
