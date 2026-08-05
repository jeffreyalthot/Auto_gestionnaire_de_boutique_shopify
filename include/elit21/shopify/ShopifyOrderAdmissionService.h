#pragma once
#include "elit21/config/Config.h"
#include "elit21/core/Result.h"
#include "elit21/core/Types.h"
#include "elit21/json/Json.h"
#include "elit21/storage/Database.h"
#include <string>
#include <vector>
namespace elit21::shopify {
enum class ShopifyOrderAdmissionDecision {accepted,duplicate,manual_review,rejected};
struct ShopifyOrderLineAdmission {std::string shopify_line_id,sku,aliexpress_product_id,aliexpress_sku_id;int quantity{0},supplier_stock{0},available_to_sell{0};double charged_unit_price_cad{0},required_unit_price_cad{0},estimated_unit_profit_cad{0};bool mapped{false},stock_ready{false},margin_ready{false};std::vector<std::string>warnings;[[nodiscard]]Json toJson()const;};
struct ShopifyOrderAdmissionReport {ShopifyOrderAdmissionDecision decision{ShopifyOrderAdmissionDecision::rejected};std::string order_id,status,idempotency_key;double charged_total_cad{0},required_subtotal_cad{0},estimated_profit_cad{0};int line_count{0},total_quantity{0};bool supplier_task_enqueued{false},review_task_enqueued{false};std::vector<ShopifyOrderLineAdmission>lines;std::vector<std::string>warnings;[[nodiscard]]bool accepted()const noexcept{return decision==ShopifyOrderAdmissionDecision::accepted;}[[nodiscard]]Json toJson()const;};
class ShopifyOrderAdmissionService {
public:
 ShopifyOrderAdmissionService(Database&database,PricingConfig pricing,InventoryConfig inventory,RiskConfig risk);
 Result<ShopifyOrderAdmissionReport> admit(const CustomerOrder&order,bool dry_run,bool require_persisted_mapping=true);
 Result<ShopifyOrderAdmissionReport> evaluate(const CustomerOrder&order,bool dry_run,bool require_persisted_mapping=true)const;
 static std::string decisionName(ShopifyOrderAdmissionDecision decision);
private:
 Result<ShopifyOrderLineAdmission> evaluateLine(const OrderLine&line,bool require_persisted_mapping)const;
 static std::string canonicalOrderPayload(const CustomerOrder&order,const ShopifyOrderAdmissionReport&report);
 Database&database_;PricingConfig pricing_;InventoryConfig inventory_;RiskConfig risk_;
};
}
