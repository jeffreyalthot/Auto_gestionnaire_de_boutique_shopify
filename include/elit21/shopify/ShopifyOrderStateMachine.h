#pragma once
#include "elit21/core/Result.h"
#include <string>
#include <vector>
namespace elit21::shopify {
enum class ShopifyOrderState {received,validating,manual_review,pending_supplier,supplier_ordered,partially_fulfilled,fulfilled,cancellation_requested,cancelled,refund_received,refunded,failed,unknown};
class ShopifyOrderStateMachine {
public:
 static ShopifyOrderState parse(const std::string& value) noexcept;
 static std::string name(ShopifyOrderState state);
 static bool terminal(ShopifyOrderState state) noexcept;
 static bool canTransition(ShopifyOrderState from, ShopifyOrderState to) noexcept;
 static Result<void> validateTransition(const std::string& from,const std::string& to);
 static std::vector<std::string> allowedNext(const std::string& from);
};
}
