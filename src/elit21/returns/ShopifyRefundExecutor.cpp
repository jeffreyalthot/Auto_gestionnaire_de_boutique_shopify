#include "elit21/returns/ShopifyRefundExecutor.h"

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
