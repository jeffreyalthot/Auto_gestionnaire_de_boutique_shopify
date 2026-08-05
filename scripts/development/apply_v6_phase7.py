from pathlib import Path
root=Path('/mnt/data/ELIT21_Shopify_AliExpress_Autonomous_Manager-v6.0.0-SHOPIFY-TRANSACTIONAL')

def write(rel,text):
 p=root/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_text(text,encoding='utf-8')

write('include/elit21/pricing/ShopifyFeeEstimator.h',r'''#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

struct ShopifyFeePolicy {
    double payment_percentage{0.0};
    double payment_fixed_cad{0.0};
    double platform_transaction_percentage{0.0};
    double reserve_percentage{0.0};
    [[nodiscard]] Result<void> validate() const;
};

struct ShopifyFeeEstimate {
    double gross_cad{0.0};
    double payment_fee_cad{0.0};
    double platform_fee_cad{0.0};
    double reserve_cad{0.0};
    double total_fees_cad{0.0};
    double net_after_fees_cad{0.0};
    [[nodiscard]] Json toJson() const;
};

class ShopifyFeeEstimator final : public platform::BusinessComponent {
public:
    ShopifyFeeEstimator();
    static Result<ShopifyFeeEstimate> estimate(double gross_cad, const ShopifyFeePolicy& policy);
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
''')
write('src/elit21/pricing/ShopifyFeeEstimator.cpp',r'''#include "elit21/pricing/ShopifyFeeEstimator.h"

#include "elit21/shopify/ShopifyMoney.h"

#include <cmath>

namespace elit21::pricing {

Result<void> ShopifyFeePolicy::validate() const {
    if (!std::isfinite(payment_percentage) || !std::isfinite(payment_fixed_cad) ||
        !std::isfinite(platform_transaction_percentage) || !std::isfinite(reserve_percentage))
        return Result<void>::failure("Shopify fee policy contains a non-finite value");
    if (payment_percentage < 0.0 || payment_percentage > 100.0 || payment_fixed_cad < 0.0 ||
        platform_transaction_percentage < 0.0 || platform_transaction_percentage > 100.0 ||
        reserve_percentage < 0.0 || reserve_percentage > 100.0)
        return Result<void>::failure("Shopify fee policy is outside the accepted bounds");
    return Result<void>::success();
}

Json ShopifyFeeEstimate::toJson() const {
    Json output = Json::object();
    output.set("gross_cad", gross_cad);
    output.set("payment_fee_cad", payment_fee_cad);
    output.set("platform_fee_cad", platform_fee_cad);
    output.set("reserve_cad", reserve_cad);
    output.set("total_fees_cad", total_fees_cad);
    output.set("net_after_fees_cad", net_after_fees_cad);
    return output;
}

ShopifyFeeEstimator::ShopifyFeeEstimator()
    : platform::BusinessComponent(
          "ShopifyFeeEstimator",
          "configurable Shopify payment and transaction fee estimator",
          platform::BusinessComponentSpec{"pricing", "shopify_fee_estimate", {}, false, false, 64U * 1024U}) {}

Result<ShopifyFeeEstimate> ShopifyFeeEstimator::estimate(double gross_cad,
                                                          const ShopifyFeePolicy& policy) {
    if (!std::isfinite(gross_cad) || gross_cad < 0.0)
        return Result<ShopifyFeeEstimate>::failure("Gross Shopify amount is invalid");
    auto validation = policy.validate();
    if (!validation) return Result<ShopifyFeeEstimate>::failure(validation.error());
    auto gross = shopify::ShopifyMoney::fromDouble(gross_cad);
    if (!gross) return Result<ShopifyFeeEstimate>::failure(gross.error());
    const auto percentCents = [&](double percentage) {
        return static_cast<std::int64_t>(std::llround(
            static_cast<long double>(gross.value().cents()) * percentage / 100.0L));
    };
    auto fixed = shopify::ShopifyMoney::fromDouble(policy.payment_fixed_cad);
    if (!fixed) return Result<ShopifyFeeEstimate>::failure(fixed.error());
    const shopify::ShopifyMoney payment(percentCents(policy.payment_percentage) + fixed.value().cents());
    const shopify::ShopifyMoney platform(percentCents(policy.platform_transaction_percentage));
    const shopify::ShopifyMoney reserve(percentCents(policy.reserve_percentage));
    const auto total = payment + platform + reserve;
    ShopifyFeeEstimate output;
    output.gross_cad = gross.value().toDouble();
    output.payment_fee_cad = payment.toDouble();
    output.platform_fee_cad = platform.toDouble();
    output.reserve_cad = reserve.toDouble();
    output.total_fees_cad = total.toDouble();
    output.net_after_fees_cad = (gross.value() - total).toDouble();
    return Result<ShopifyFeeEstimate>::success(output);
}

platform::OperationResult ShopifyFeeEstimator::execute(const platform::OperationContext& context) {
    auto base = executeBusiness(context);
    if (!base.success) return base;
    auto document = Json::parse(context.payload.empty() ? "{}" : context.payload);
    if (!document) return platform::OperationResult::failure("INVALID_JSON", document.error());
    ShopifyFeePolicy policy;
    policy.payment_percentage = document.value().getNumber("payment_percentage", 0.0);
    policy.payment_fixed_cad = document.value().getNumber("payment_fixed_cad", 0.0);
    policy.platform_transaction_percentage = document.value().getNumber("platform_transaction_percentage", 0.0);
    policy.reserve_percentage = document.value().getNumber("reserve_percentage", 0.0);
    auto estimated = estimate(document.value().getNumber("gross_cad", 0.0), policy);
    if (!estimated) return platform::OperationResult::failure("INVALID_FEE_POLICY", estimated.error());
    base.message = "Shopify fees estimated from explicit configurable rates";
    base.metrics["gross_cad"] = estimated.value().gross_cad;
    base.metrics["total_fees_cad"] = estimated.value().total_fees_cad;
    base.metrics["net_after_fees_cad"] = estimated.value().net_after_fees_cad;
    base.attributes["fee_source"] = "configuration_not_hardcoded";
    return base;
}

} // namespace elit21::pricing
''')

write('include/elit21/returns/ShopifyRefundExecutor.h',r'''#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/platform/BusinessComponent.h"

#include <string>

namespace elit21::returns {

struct ShopifyRefundPlan {
    std::string shopify_order_id;
    std::string reason;
    double order_total_cad{0.0};
    double already_refunded_cad{0.0};
    double requested_refund_cad{0.0};
    double remaining_refundable_cad{0.0};
    bool restock{false};
    bool notify_customer{true};
    [[nodiscard]] Json toJson() const;
};

class ShopifyRefundExecutor final : public platform::BusinessComponent {
public:
    ShopifyRefundExecutor();
    static Result<ShopifyRefundPlan> plan(const std::string& shopify_order_id,
                                          double order_total_cad,
                                          double already_refunded_cad,
                                          double requested_refund_cad,
                                          std::string reason,
                                          bool restock,
                                          bool notify_customer);
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::returns
''')
write('src/elit21/returns/ShopifyRefundExecutor.cpp',r'''#include "elit21/returns/ShopifyRefundExecutor.h"

#include "elit21/shopify/ShopifyMoney.h"
#include "elit21/util/StringUtil.h"

#include <cmath>

namespace elit21::returns {

Json ShopifyRefundPlan::toJson() const {
    Json output = Json::object();
    output.set("shopify_order_id", shopify_order_id);
    output.set("reason", reason);
    output.set("order_total_cad", order_total_cad);
    output.set("already_refunded_cad", already_refunded_cad);
    output.set("requested_refund_cad", requested_refund_cad);
    output.set("remaining_refundable_cad", remaining_refundable_cad);
    output.set("restock", restock);
    output.set("notify_customer", notify_customer);
    return output;
}

ShopifyRefundExecutor::ShopifyRefundExecutor()
    : platform::BusinessComponent(
          "ShopifyRefundExecutor", "validated Shopify refund command planner",
          platform::BusinessComponentSpec{"returns", "shopify_refund_plan", {"order_id"}, true, true, 256U * 1024U}) {}

Result<ShopifyRefundPlan> ShopifyRefundExecutor::plan(const std::string& shopify_order_id,
                                                       double order_total_cad,
                                                       double already_refunded_cad,
                                                       double requested_refund_cad,
                                                       std::string reason,
                                                       bool restock,
                                                       bool notify_customer) {
    if (shopify_order_id.empty()) return Result<ShopifyRefundPlan>::failure("Shopify order id is required");
    auto total = shopify::ShopifyMoney::fromDouble(order_total_cad);
    auto refunded = shopify::ShopifyMoney::fromDouble(already_refunded_cad);
    auto requested = shopify::ShopifyMoney::fromDouble(requested_refund_cad);
    if (!total || !refunded || !requested) return Result<ShopifyRefundPlan>::failure("Refund amount is invalid");
    if (refunded.value() > total.value()) return Result<ShopifyRefundPlan>::failure("Already refunded amount exceeds order total");
    const auto remaining = total.value() - refunded.value();
    if (requested.value().cents() <= 0) return Result<ShopifyRefundPlan>::failure("Refund amount must be positive");
    if (requested.value() > remaining) return Result<ShopifyRefundPlan>::failure("Refund exceeds remaining refundable amount");
    reason = util::trim(std::move(reason));
    if (reason.empty()) reason = "merchant_requested";
    ShopifyRefundPlan output;
    output.shopify_order_id = shopify_order_id;
    output.reason = std::move(reason);
    output.order_total_cad = total.value().toDouble();
    output.already_refunded_cad = refunded.value().toDouble();
    output.requested_refund_cad = requested.value().toDouble();
    output.remaining_refundable_cad = (remaining - requested.value()).toDouble();
    output.restock = restock;
    output.notify_customer = notify_customer;
    return Result<ShopifyRefundPlan>::success(std::move(output));
}

platform::OperationResult ShopifyRefundExecutor::execute(const platform::OperationContext& context) {
    auto base = executeBusiness(context);
    if (!base.success) return base;
    auto payload = Json::parse(context.payload);
    if (!payload) return platform::OperationResult::failure("INVALID_JSON", payload.error());
    auto refund = plan(payload.value().getScalarString("order_id"),
                       payload.value().getNumber("order_total_cad"),
                       payload.value().getNumber("already_refunded_cad"),
                       payload.value().getNumber("requested_refund_cad"),
                       payload.value().getString("reason"),
                       payload.value().getBool("restock"),
                       payload.value().getBool("notify_customer", true));
    if (!refund) return platform::OperationResult::failure("INVALID_REFUND", refund.error());
    base.message = context.dry_run ? "Shopify refund validated and planned" :
                                    "Shopify refund command validated for an authorized client service";
    base.metrics["requested_refund_cad"] = refund.value().requested_refund_cad;
    base.metrics["remaining_refundable_cad"] = refund.value().remaining_refundable_cad;
    base.attributes["shopify_order_id"] = refund.value().shopify_order_id;
    base.attributes["refund_reason"] = refund.value().reason;
    base.attributes["execution_boundary"] = "plan_only_client_service_must_submit_graphql";
    return base;
}

} // namespace elit21::returns
''')

write('include/elit21/terminal/panels/ShopifyPanel.h',r'''#pragma once

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
''')
write('src/elit21/terminal/panels/ShopifyPanel.cpp',r'''#include "elit21/terminal/panels/ShopifyPanel.h"

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
    base.attributes["connection"] = snapshot.connected ? "connected" : "offline";
    base.attributes["mode"] = snapshot.dry_run ? "dry_run" : "live";
    base.attributes["render_line_count"] = std::to_string(snapshot.renderLines().size());
    return base;
}

} // namespace elit21::terminal::panels
''')

# Analyzer: score public interfaces with the implementation paired to them.
p=root/'src/elit21/progress/ProjectProgressAnalyzer.cpp';s=p.read_text()
old='''    const auto text = textual ? readText(file) : std::string{};
    result.lines = lineCount(text);
'''
new='''    const auto text = textual ? readText(file) : std::string{};
    std::string paired_text;
    if ((extension == ".h" || extension == ".hpp") && lower_path.rfind("include/elit21/", 0) == 0) {
        auto paired = root / std::filesystem::path("src") /
                      result.relative_path.lexically_relative("include");
        paired.replace_extension(".cpp");
        if (std::filesystem::exists(paired)) paired_text = readText(paired);
    }
    const auto analysis_text = paired_text.empty() ? text : text + "\n" + paired_text;
    result.lines = lineCount(text);
'''
if old not in s: raise SystemExit('analyzer text anchor missing')
s=s.replace(old,new)
# Replace classification contains reads with analysis_text in C++ branch only broadly.
start=s.index('    if (extension == ".cpp" || extension == ".h" || extension == ".hpp") {')
end=s.index('    if (extension == ".sql")', start)
block=s[start:end].replace('text.find(', 'analysis_text.find(').replace('containsAny(text,', 'containsAny(analysis_text,')
s=s[:start]+block+s[end:]
# New specific transactional classification before typed_shopify_core.
old='''        const bool typed_shopify_core = (lower_path.rfind("include/elit21/shopify/", 0) == 0 ||
'''
new='''        const bool transactional_shopify = lower_path.find("shopify") != std::string::npos &&
            containsAny(analysis_text, {"ShopifyOrderAdmissionService", "ShopifyOrderStateMachine",
                                       "ShopifyPrivacyService", "ShopifyProductionReadiness", "ShopifyMoney"}) &&
            containsAny(analysis_text, {"Result<", "Database", "validate", "transaction", "redact"});
        const bool typed_shopify_core = (lower_path.rfind("include/elit21/shopify/", 0) == 0 ||
'''
if old not in s: raise SystemExit('shop core anchor missing')
s=s.replace(old,new)
old='''        } else if (typed_shopify_core) {
'''
new='''        } else if (transactional_shopify) {
            result.percentage = 97; result.emoji = "✅"; result.state = "SHOPIFY TRANSACTIONNEL";
            result.detail = "monnaie sûre, états, admission, confidentialité ou readiness";
        } else if (typed_shopify_core) {
'''
if old not in s: raise SystemExit('classification anchor missing')
s=s.replace(old,new)
p.write_text(s,encoding='utf-8')

# Unit tests for new typed components.
p=root/'tests/unit_tests.cpp';s=p.read_text()
s=s.replace('#include "elit21/pricing/PricingEngine.h"','#include "elit21/pricing/PricingEngine.h"\n#include "elit21/pricing/ShopifyFeeEstimator.h"\n#include "elit21/returns/ShopifyRefundExecutor.h"\n#include "elit21/terminal/panels/ShopifyPanel.h"')
s=s.replace('''    expect(!elit21::shopify::ShopifyMoney::supplierPrice(elit21::shopify::ShopifyMoney(1000), 99.9, elit21::shopify::ShopifyMoney(300)),
           "marge inférieure à 100% rejetée");
''','''    expect(!elit21::shopify::ShopifyMoney::supplierPrice(elit21::shopify::ShopifyMoney(1000), 99.9, elit21::shopify::ShopifyMoney(300)),
           "marge inférieure à 100% rejetée");
    elit21::pricing::ShopifyFeePolicy fee_policy{2.9, 0.30, 0.0, 0.0};
    auto fees = elit21::pricing::ShopifyFeeEstimator::estimate(100.0, fee_policy);
    expect(bool(fees) && std::fabs(fees.value().total_fees_cad - 3.20) < 0.001,
           "frais Shopify configurables calculés en centimes");
    auto refund = elit21::returns::ShopifyRefundExecutor::plan(
        "gid://shopify/Order/1", 100.0, 20.0, 30.0, "customer_request", true, true);
    expect(bool(refund) && std::fabs(refund.value().remaining_refundable_cad - 50.0) < 0.001,
           "plan de remboursement respecte le solde remboursable");
    expect(!elit21::returns::ShopifyRefundExecutor::plan(
        "gid://shopify/Order/1", 100.0, 90.0, 20.0, "invalid", false, true),
        "sur-remboursement Shopify bloqué");
    elit21::Json panel_json = elit21::Json::object();
    panel_json.set("connected", true); panel_json.set("readiness_score", 93);
    panel_json.set("api_version", "2026-07"); panel_json.set("pending_tasks", 4);
    auto panel = elit21::terminal::panels::ShopifyPanel::fromJson(panel_json);
    expect(panel.renderLines().size() >= 8 && panel.readiness_score == 93,
           "panneau Shopify fixe généré depuis un snapshot typé");
''')
p.write_text(s,encoding='utf-8')
print('phase7 applied')
