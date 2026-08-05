#include "elit21/terminal/panels/ShopifyPanel.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace elit21::terminal::panels {
namespace {
std::string line(std::string label, std::string value, std::size_t width) {
    if (width < 24) width = 24;
    const auto available = width > label.size() + 5 ? width - label.size() - 5 : 1;
    if (value.size() > available) value.resize(available);
    std::ostringstream output;
    output << "║ " << std::left << std::setw(static_cast<int>(width / 2 - 2)) << label
           << " : " << std::left << std::setw(static_cast<int>(width - width / 2 - 3)) << value << "║";
    return output.str();
}
} // namespace

Json ShopifyPanelSnapshot::toJson() const {
    Json output = Json::object();
    output.set("connected", connected); output.set("dry_run", dry_run);
    output.set("readiness_score", readiness_score);
    output.set("graphql_requests", static_cast<std::int64_t>(graphql_requests));
    output.set("graphql_retries", static_cast<std::int64_t>(graphql_retries));
    output.set("graphql_throttles", static_cast<std::int64_t>(graphql_throttles));
    output.set("webhooks_accepted", static_cast<std::int64_t>(webhooks_accepted));
    output.set("webhooks_rejected", static_cast<std::int64_t>(webhooks_rejected));
    output.set("pending_tasks", static_cast<std::int64_t>(pending_tasks));
    output.set("manual_review_orders", static_cast<std::int64_t>(manual_review_orders));
    output.set("webhook_p90_response_ms", webhook_p90_response_ms);
    output.set("webhook_failure_rate_percent", webhook_failure_rate_percent);
    output.set("api_contract_score", api_contract_score);
    output.set("circuit_state", circuit_state);
    output.set("api_version", api_version); output.set("last_event", last_event);
    return output;
}

std::vector<std::string> ShopifyPanelSnapshot::renderLines(std::size_t width) const {
    width = std::clamp<std::size_t>(width, 48, 160);
    const std::string border = "╠" + std::string(width - 2, '=') + "╣";
    return {border,
            line("Shopify", connected ? "CONNECTE" : "HORS LIGNE", width),
            line("Mode", dry_run ? "DRY RUN" : "LIVE", width),
            line("Version API", api_version.empty() ? "inconnue" : api_version, width),
            line("Readiness", std::to_string(readiness_score) + "%", width),
            line("GraphQL req/retry/throttle", std::to_string(graphql_requests) + "/" +
                 std::to_string(graphql_retries) + "/" + std::to_string(graphql_throttles), width),
            line("Circuit GraphQL / contrat", circuit_state + " / " +
                 std::to_string(api_contract_score) + "%", width),
            line("Webhook p90 / echec", std::to_string(static_cast<int>(webhook_p90_response_ms)) +
                 " ms / " + std::to_string(webhook_failure_rate_percent) + "%", width),
            line("Webhooks acceptes/rejetes", std::to_string(webhooks_accepted) + "/" +
                 std::to_string(webhooks_rejected), width),
            line("Taches / revision manuelle", std::to_string(pending_tasks) + "/" +
                 std::to_string(manual_review_orders), width),
            line("Dernier evenement", last_event.empty() ? "aucun" : last_event, width)};
}

ShopifyPanel::ShopifyPanel()
    : platform::BusinessComponent(
          "ShopifyPanel", "fixed-position Shopify operations panel",
          platform::BusinessComponentSpec{"terminal", "shopify_dashboard_snapshot", {}, false, false, 256U * 1024U}) {}

ShopifyPanelSnapshot ShopifyPanel::fromJson(const Json& value) {
    ShopifyPanelSnapshot output;
    output.connected = value.getBool("connected"); output.dry_run = value.getBool("dry_run", true);
    output.readiness_score = std::clamp(value.getInt("readiness_score"), 0, 100);
    output.graphql_requests = static_cast<std::uint64_t>(std::max<std::int64_t>(0, value.getInt64("graphql_requests")));
    output.graphql_retries = static_cast<std::uint64_t>(std::max<std::int64_t>(0, value.getInt64("graphql_retries")));
    output.graphql_throttles = static_cast<std::uint64_t>(std::max<std::int64_t>(0, value.getInt64("graphql_throttles")));
    output.webhooks_accepted = static_cast<std::uint64_t>(std::max<std::int64_t>(0, value.getInt64("webhooks_accepted")));
    output.webhooks_rejected = static_cast<std::uint64_t>(std::max<std::int64_t>(0, value.getInt64("webhooks_rejected")));
    output.pending_tasks = static_cast<std::uint64_t>(std::max<std::int64_t>(0, value.getInt64("pending_tasks")));
    output.manual_review_orders = static_cast<std::uint64_t>(std::max<std::int64_t>(0, value.getInt64("manual_review_orders")));
    output.webhook_p90_response_ms = std::max(0.0, value.getNumber("webhook_p90_response_ms"));
    output.webhook_failure_rate_percent = std::max(0.0, value.getNumber("webhook_failure_rate_percent"));
    output.api_contract_score = std::clamp(value.getInt("api_contract_score"), 0, 100);
    output.circuit_state = value.getString("circuit_state", "closed");
    output.api_version = value.getString("api_version"); output.last_event = value.getString("last_event");
    return output;
}

platform::OperationResult ShopifyPanel::execute(const platform::OperationContext& context) {
    auto base = executeBusiness(context);
    if (!base.success) return base;
    auto payload = Json::parse(context.payload.empty() ? "{}" : context.payload);
    if (!payload) return platform::OperationResult::failure("INVALID_JSON", payload.error());
    const auto snapshot = fromJson(payload.value());
    base.message = "Shopify fixed-position panel snapshot prepared";
    base.metrics["readiness_score"] = snapshot.readiness_score;
    base.metrics["pending_tasks"] = static_cast<double>(snapshot.pending_tasks);
    base.metrics["manual_review_orders"] = static_cast<double>(snapshot.manual_review_orders);
    base.metrics["webhook_p90_response_ms"] = snapshot.webhook_p90_response_ms;
    base.metrics["webhook_failure_rate_percent"] = snapshot.webhook_failure_rate_percent;
    base.metrics["api_contract_score"] = snapshot.api_contract_score;
    base.attributes["connection"] = snapshot.connected ? "connected" : "offline";
    base.attributes["mode"] = snapshot.dry_run ? "dry_run" : "live";
    base.attributes["circuit_state"] = snapshot.circuit_state;
    base.attributes["render_line_count"] = std::to_string(snapshot.renderLines().size());
    return base;
}

} // namespace elit21::terminal::panels
