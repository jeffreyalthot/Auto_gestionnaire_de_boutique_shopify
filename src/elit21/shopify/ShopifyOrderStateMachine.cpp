#include "elit21/shopify/ShopifyOrderStateMachine.h"
#include "elit21/util/StringUtil.h"

#include <array>
#include <utility>

namespace elit21::shopify {
namespace {
using Transition = std::pair<ShopifyOrderState, ShopifyOrderState>;
constexpr std::array<Transition, 25> transitions{{
    {ShopifyOrderState::received, ShopifyOrderState::validating},
    {ShopifyOrderState::received, ShopifyOrderState::manual_review},
    {ShopifyOrderState::received, ShopifyOrderState::cancelled},
    {ShopifyOrderState::validating, ShopifyOrderState::pending_supplier},
    {ShopifyOrderState::validating, ShopifyOrderState::manual_review},
    {ShopifyOrderState::validating, ShopifyOrderState::failed},
    {ShopifyOrderState::manual_review, ShopifyOrderState::pending_supplier},
    {ShopifyOrderState::manual_review, ShopifyOrderState::cancelled},
    {ShopifyOrderState::manual_review, ShopifyOrderState::failed},
    {ShopifyOrderState::pending_supplier, ShopifyOrderState::supplier_ordered},
    {ShopifyOrderState::pending_supplier, ShopifyOrderState::manual_review},
    {ShopifyOrderState::pending_supplier, ShopifyOrderState::cancellation_requested},
    {ShopifyOrderState::pending_supplier, ShopifyOrderState::cancelled},
    {ShopifyOrderState::pending_supplier, ShopifyOrderState::failed},
    {ShopifyOrderState::supplier_ordered, ShopifyOrderState::partially_fulfilled},
    {ShopifyOrderState::supplier_ordered, ShopifyOrderState::fulfilled},
    {ShopifyOrderState::supplier_ordered, ShopifyOrderState::cancellation_requested},
    {ShopifyOrderState::supplier_ordered, ShopifyOrderState::refund_received},
    {ShopifyOrderState::partially_fulfilled, ShopifyOrderState::fulfilled},
    {ShopifyOrderState::partially_fulfilled, ShopifyOrderState::refund_received},
    {ShopifyOrderState::fulfilled, ShopifyOrderState::refund_received},
    {ShopifyOrderState::cancellation_requested, ShopifyOrderState::cancelled},
    {ShopifyOrderState::refund_received, ShopifyOrderState::refunded},
    {ShopifyOrderState::failed, ShopifyOrderState::manual_review},
    {ShopifyOrderState::failed, ShopifyOrderState::cancelled}
}};
} // namespace

ShopifyOrderState ShopifyOrderStateMachine::parse(const std::string& value) noexcept {
    const auto normalized = util::lower(util::trim(value));
    if (normalized == "received") return ShopifyOrderState::received;
    if (normalized == "validating") return ShopifyOrderState::validating;
    if (normalized == "manual_review" || normalized == "manual-review") return ShopifyOrderState::manual_review;
    if (normalized == "pending_supplier" || normalized == "dry_run_pending_supplier") return ShopifyOrderState::pending_supplier;
    if (normalized == "supplier_ordered") return ShopifyOrderState::supplier_ordered;
    if (normalized == "partially_fulfilled") return ShopifyOrderState::partially_fulfilled;
    if (normalized == "fulfilled" || normalized == "delivered") return ShopifyOrderState::fulfilled;
    if (normalized == "cancellation_requested") return ShopifyOrderState::cancellation_requested;
    if (normalized == "cancelled" || normalized == "canceled") return ShopifyOrderState::cancelled;
    if (normalized == "refund_received" || normalized == "refund_pending") return ShopifyOrderState::refund_received;
    if (normalized == "refunded") return ShopifyOrderState::refunded;
    if (normalized == "failed" || normalized == "dead") return ShopifyOrderState::failed;
    return ShopifyOrderState::unknown;
}

std::string ShopifyOrderStateMachine::name(ShopifyOrderState state) {
    switch (state) {
        case ShopifyOrderState::received: return "received";
        case ShopifyOrderState::validating: return "validating";
        case ShopifyOrderState::manual_review: return "manual_review";
        case ShopifyOrderState::pending_supplier: return "pending_supplier";
        case ShopifyOrderState::supplier_ordered: return "supplier_ordered";
        case ShopifyOrderState::partially_fulfilled: return "partially_fulfilled";
        case ShopifyOrderState::fulfilled: return "fulfilled";
        case ShopifyOrderState::cancellation_requested: return "cancellation_requested";
        case ShopifyOrderState::cancelled: return "cancelled";
        case ShopifyOrderState::refund_received: return "refund_received";
        case ShopifyOrderState::refunded: return "refunded";
        case ShopifyOrderState::failed: return "failed";
        default: return "unknown";
    }
}

bool ShopifyOrderStateMachine::terminal(ShopifyOrderState state) noexcept {
    return state == ShopifyOrderState::cancelled || state == ShopifyOrderState::refunded;
}

bool ShopifyOrderStateMachine::canTransition(ShopifyOrderState from, ShopifyOrderState to) noexcept {
    if (from == to && from != ShopifyOrderState::unknown) return true;
    for (const auto& transition : transitions) {
        if (transition.first == from && transition.second == to) return true;
    }
    return false;
}

Result<void> ShopifyOrderStateMachine::validateTransition(const std::string& from,
                                                          const std::string& to) {
    const auto source = parse(from);
    const auto destination = parse(to);
    if (source == ShopifyOrderState::unknown)
        return Result<void>::failure("Unknown Shopify order source state: " + from);
    if (destination == ShopifyOrderState::unknown)
        return Result<void>::failure("Unknown Shopify order destination state: " + to);
    if (!canTransition(source, destination))
        return Result<void>::failure("Invalid Shopify order transition: " + name(source) + " -> " + name(destination));
    return Result<void>::success();
}

std::vector<std::string> ShopifyOrderStateMachine::allowedNext(const std::string& from) {
    const auto source = parse(from);
    std::vector<std::string> output;
    for (const auto& transition : transitions) {
        if (transition.first == source) output.push_back(name(transition.second));
    }
    return output;
}

} // namespace elit21::shopify
