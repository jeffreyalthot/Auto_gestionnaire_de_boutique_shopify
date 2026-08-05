from pathlib import Path
ROOT=Path('/mnt/data/ELIT21_Shopify_AliExpress_Autonomous_Manager-v6.0.0-SHOPIFY-TRANSACTIONAL')
def write(rel,c):
 p=ROOT/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_text(c,encoding='utf-8')
def replace(rel,old,new,count=1):
 p=ROOT/rel;t=p.read_text(encoding='utf-8');
 if old not in t: raise RuntimeError(f'missing in {rel}: {old[:100]}')
 p.write_text(t.replace(old,new,count),encoding='utf-8')

write('include/elit21/shopify/ShopifyOrderAdmissionService.h', r'''#pragma once
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
''')

write('src/elit21/shopify/ShopifyOrderAdmissionService.cpp', r'''#include "elit21/shopify/ShopifyOrderAdmissionService.h"
#include "elit21/security/Crypto.h"
#include "elit21/shopify/ShopifyMoney.h"
#include "elit21/util/StringUtil.h"
#include <algorithm>
#include <cmath>
#include <set>
namespace elit21::shopify { namespace {std::string currency(std::string v){return util::upper(util::trim(std::move(v)));}Json lineJson(const OrderLine&l){Json j=Json::object();j.set("shopify_line_id",l.shopify_line_id);j.set("shopify_variant_id",l.shopify_variant_id);j.set("aliexpress_product_id",l.aliexpress_product_id);j.set("aliexpress_sku_id",l.aliexpress_sku_id);j.set("sku",l.sku);j.set("title",l.title);j.set("quantity",l.quantity);j.set("unit_price_cad",l.unit_price_cad);return j;}}
Json ShopifyOrderLineAdmission::toJson()const{Json o=Json::object();o.set("shopify_line_id",shopify_line_id);o.set("sku",sku);o.set("aliexpress_product_id",aliexpress_product_id);o.set("aliexpress_sku_id",aliexpress_sku_id);o.set("quantity",quantity);o.set("supplier_stock",supplier_stock);o.set("available_to_sell",available_to_sell);o.set("charged_unit_price_cad",charged_unit_price_cad);o.set("required_unit_price_cad",required_unit_price_cad);o.set("estimated_unit_profit_cad",estimated_unit_profit_cad);o.set("mapped",mapped);o.set("stock_ready",stock_ready);o.set("margin_ready",margin_ready);Json a=Json::array();for(const auto&w:warnings){Json x=Json::object();x.set("message",w);a.push(x);}o.set("warnings",a);return o;}
std::string ShopifyOrderAdmissionService::decisionName(ShopifyOrderAdmissionDecision d){switch(d){case ShopifyOrderAdmissionDecision::accepted:return"accepted";case ShopifyOrderAdmissionDecision::duplicate:return"duplicate";case ShopifyOrderAdmissionDecision::manual_review:return"manual_review";default:return"rejected";}}
Json ShopifyOrderAdmissionReport::toJson()const{Json o=Json::object();o.set("decision",ShopifyOrderAdmissionService::decisionName(decision));o.set("order_id",order_id);o.set("status",status);o.set("idempotency_key",idempotency_key);o.set("charged_total_cad",charged_total_cad);o.set("required_subtotal_cad",required_subtotal_cad);o.set("estimated_profit_cad",estimated_profit_cad);o.set("line_count",line_count);o.set("total_quantity",total_quantity);o.set("supplier_task_enqueued",supplier_task_enqueued);o.set("review_task_enqueued",review_task_enqueued);Json ls=Json::array();for(const auto&l:lines)ls.push(l.toJson());o.set("lines",ls);Json ws=Json::array();for(const auto&w:warnings){Json x=Json::object();x.set("message",w);ws.push(x);}o.set("warnings",ws);return o;}
ShopifyOrderAdmissionService::ShopifyOrderAdmissionService(Database&d,PricingConfig p,InventoryConfig i,RiskConfig r):database_(d),pricing_(std::move(p)),inventory_(std::move(i)),risk_(std::move(r)){}
Result<ShopifyOrderLineAdmission> ShopifyOrderAdmissionService::evaluateLine(const OrderLine&l,bool require)const{ShopifyOrderLineAdmission r;r.shopify_line_id=l.shopify_line_id;r.sku=l.sku;r.aliexpress_product_id=l.aliexpress_product_id;r.aliexpress_sku_id=l.aliexpress_sku_id;r.quantity=l.quantity;r.charged_unit_price_cad=l.unit_price_cad;if(l.quantity<=0)return Result<ShopifyOrderLineAdmission>::failure("Order line quantity must be positive");if(!std::isfinite(l.unit_price_cad)||l.unit_price_cad<0)return Result<ShopifyOrderLineAdmission>::failure("Order line price is invalid");if(l.aliexpress_product_id.empty()||l.aliexpress_sku_id.empty()){r.warnings.push_back("AliExpress mapping is missing");return Result<ShopifyOrderLineAdmission>::success(std::move(r));}auto stored=database_.variantByAliExpressSku(l.aliexpress_product_id,l.aliexpress_sku_id);if(!stored)return Result<ShopifyOrderLineAdmission>::failure(stored.error());if(!stored.value()){if(require)r.warnings.push_back("AliExpress SKU mapping is not persisted locally");else r.mapped=r.stock_ready=r.margin_ready=true;return Result<ShopifyOrderLineAdmission>::success(std::move(r));}const auto&v=*stored.value();r.mapped=true;r.supplier_stock=v.stock;r.available_to_sell=std::max(0,v.stock-std::max(0,inventory_.safety_buffer));r.stock_ready=l.quantity<=r.available_to_sell;if(!r.stock_ready)r.warnings.push_back("Supplier stock is below the requested quantity after safety buffer");auto cost=ShopifyMoney::fromDouble(v.cost),ship=ShopifyMoney::fromDouble(v.shipping),charged=ShopifyMoney::fromDouble(l.unit_price_cad);if(!cost||!ship||!charged)return Result<ShopifyOrderLineAdmission>::failure("Order line monetary value cannot be represented safely");auto requiredPrice=ShopifyMoney::supplierPrice(cost.value(),pricing_.markup_percent_before_shipping,ship.value());if(!requiredPrice)return Result<ShopifyOrderLineAdmission>::failure(requiredPrice.error());r.required_unit_price_cad=requiredPrice.value().toDouble();r.margin_ready=charged.value()>=requiredPrice.value();if(!r.margin_ready)r.warnings.push_back("Charged price is below supplier cost plus 100% markup before shipping");r.estimated_unit_profit_cad=(charged.value()-(cost.value()+ship.value())).toDouble();return Result<ShopifyOrderLineAdmission>::success(std::move(r));}
Result<ShopifyOrderAdmissionReport> ShopifyOrderAdmissionService::evaluate(const CustomerOrder&o,bool,bool require)const{ShopifyOrderAdmissionReport r;r.order_id=o.shopify_order_id;r.charged_total_cad=o.total_cad;r.line_count=int(o.lines.size());if(!o.valid())return Result<ShopifyOrderAdmissionReport>::failure("Shopify order model is invalid");if(currency(o.currency)!="CAD")return Result<ShopifyOrderAdmissionReport>::failure("Only CAD Shopify orders are supported");if(!std::isfinite(o.total_cad)||o.total_cad<=0)return Result<ShopifyOrderAdmissionReport>::failure("Shopify order total is invalid");if(o.total_cad>risk_.maximum_order_value_cad)return Result<ShopifyOrderAdmissionReport>::failure("Shopify order exceeds the configured risk limit");if(!o.shipping_address.isCanadian()||!o.shipping_address.hasRequiredShippingFields())return Result<ShopifyOrderAdmissionReport>::failure("Shopify order has an incomplete Canadian shipping address");if(o.lines.size()>250)return Result<ShopifyOrderAdmissionReport>::failure("Shopify order exceeds 250 lines");std::set<std::string>ids;bool review=false;for(const auto&l:o.lines){if(!l.shopify_line_id.empty()&&!ids.insert(l.shopify_line_id).second)return Result<ShopifyOrderAdmissionReport>::failure("Duplicate Shopify line item identifier");auto x=evaluateLine(l,require);if(!x)return Result<ShopifyOrderAdmissionReport>::failure(x.error());r.total_quantity+=l.quantity;r.required_subtotal_cad+=x.value().required_unit_price_cad*l.quantity;r.estimated_profit_cad+=x.value().estimated_unit_profit_cad*l.quantity;if(!x.value().mapped||!x.value().stock_ready||!x.value().margin_ready)review=true;r.lines.push_back(x.take());}auto total=ShopifyMoney::fromDouble(o.total_cad);if(!total)return Result<ShopifyOrderAdmissionReport>::failure(total.error());r.idempotency_key=crypto::sha256Hex("shopify-order-v6:"+o.shopify_order_id+":"+total.value().toDecimal());if(review){r.decision=ShopifyOrderAdmissionDecision::manual_review;r.status="manual_review";r.warnings.push_back("Order requires manual review before supplier placement");}else{r.decision=ShopifyOrderAdmissionDecision::accepted;r.status="pending_supplier";}return Result<ShopifyOrderAdmissionReport>::success(std::move(r));}
std::string ShopifyOrderAdmissionService::canonicalOrderPayload(const CustomerOrder&o,const ShopifyOrderAdmissionReport&r){Json n=Json::object();n.set("id",o.shopify_order_id);n.set("name",o.order_name);n.set("email",o.email);n.set("phone",o.phone);n.set("currency",currency(o.currency));n.set("total_cad",o.total_cad);n.set("admission",r.toJson());Json a=Json::object();a.set("first_name",o.shipping_address.first_name);a.set("last_name",o.shipping_address.last_name);a.set("company",o.shipping_address.company);a.set("address1",o.shipping_address.address1);a.set("address2",o.shipping_address.address2);a.set("city",o.shipping_address.city);a.set("province",o.shipping_address.province);a.set("province_code",o.shipping_address.province_code);a.set("country",o.shipping_address.country);a.set("country_code",o.shipping_address.country_code);a.set("zip",o.shipping_address.postal_code);a.set("phone",o.shipping_address.phone);n.set("shipping_address",a);Json ls=Json::array();for(const auto&l:o.lines)ls.push(lineJson(l));n.set("line_items",ls);if(!o.raw_json.empty())n.set("source_payload_hash",crypto::sha256Hex(o.raw_json));return n.dump();}
Result<ShopifyOrderAdmissionReport> ShopifyOrderAdmissionService::admit(const CustomerOrder&o,bool dry,bool require){auto e=evaluate(o,dry,require);if(!e)return e;auto r=e.take();StoredOrder s;s.shopify_id=o.shopify_order_id;s.status=r.status;s.total=o.total_cad;s.customer_email=o.email;s.currency="CAD";s.idempotency_key=r.idempotency_key;s.payload=canonicalOrderPayload(o,r);std::vector<StoredOrderLine>ls;for(const auto&l:o.lines){StoredOrderLine x;x.shopify_line_id=l.shopify_line_id;x.shopify_variant_id=l.shopify_variant_id;x.aliexpress_product_id=l.aliexpress_product_id;x.aliexpress_sku_id=l.aliexpress_sku_id;x.sku=l.sku;x.title=l.title;x.quantity=l.quantity;x.unit_price_cad=l.unit_price_cad;x.status=r.accepted()?"admitted":"review";ls.push_back(std::move(x));}auto saved=database_.saveOrderWithLines(s,ls);if(!saved)return Result<ShopifyOrderAdmissionReport>::failure(saved.error());if(!saved.value()){r.decision=ShopifyOrderAdmissionDecision::duplicate;r.status="duplicate";return Result<ShopifyOrderAdmissionReport>::success(std::move(r));}Json task=Json::object();task.set("shopify_order_id",o.shopify_order_id);task.set("dry_run",dry);task.set("admission",r.toJson());auto kind=r.accepted()?"place_supplier_order":"review_shopify_order";auto dedupe=(r.accepted()?"supplier-order:":"order-review:")+o.shopify_order_id;auto q=database_.enqueueUniqueTask(kind,task.dump(),r.accepted()?20:5,{},dedupe);if(!q)return Result<ShopifyOrderAdmissionReport>::failure(q.error());r.supplier_task_enqueued=r.accepted()&&q.value().inserted;r.review_task_enqueued=!r.accepted()&&q.value().inserted;database_.audit(r.accepted()?"INFO":"WARNING","shopify.order.admission",r.accepted()?"Shopify order admitted":"Shopify order routed to manual review",r.toJson().dump(),o.shopify_order_id);database_.metric("shopify.order.admission.estimated_profit_cad",r.estimated_profit_cad,"{\"order_id\":\""+o.shopify_order_id+"\"}");return Result<ShopifyOrderAdmissionReport>::success(std::move(r));}
}
''')

# Rewrite OrderManager header
write('include/elit21/orders/OrderManager.h', r'''#pragma once
#include "elit21/aliexpress/AliExpressClient.h"
#include "elit21/compliance/CanadaCompliance.h"
#include "elit21/config/Config.h"
#include "elit21/core/Types.h"
#include "elit21/logging/Logger.h"
#include "elit21/risk/RiskEngine.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/shopify/ShopifyOrderAdmissionService.h"
#include "elit21/storage/Database.h"
namespace elit21 {class OrderManager{public:OrderManager(ShopifyClient&s,AliExpressClient&a,CanadaCompliance&c,RiskEngine&r,Database&d,Logger&l,Config&cfg,RuntimeCounters&n):shop_(s),ali_(a),compliance_(c),risk_(r),db_(d),log_(l),config_(cfg),admission_(d,cfg.pricing,cfg.inventory,cfg.risk),counters_(n){}void pollShopify();Result<void> ingest(CustomerOrder order);void processPending();private:Result<void> resolveMappings(CustomerOrder&order);std::string chooseShipping(const CustomerOrder&order);ShopifyClient&shop_;AliExpressClient&ali_;CanadaCompliance&compliance_;RiskEngine&risk_;Database&db_;Logger&log_;Config&config_;shopify::ShopifyOrderAdmissionService admission_;RuntimeCounters&counters_;};}
''')
# replace ingest function
p=ROOT/'src/elit21/orders/OrderManager.cpp';t=p.read_text(encoding='utf-8');start=t.index('Result<void> OrderManager::ingest(CustomerOrder order) {');end=t.index('\nstd::string OrderManager::chooseShipping',start)
new=r'''Result<void> OrderManager::ingest(CustomerOrder order) {
    if (order.shopify_order_id.empty()) return Result<void>::failure("Identifiant Shopify manquant.");
    if (order.currency != "CAD") return Result<void>::failure("Commande non CAD rejetée: " + order.currency);
    if (order.total_cad <= 0.0) return Result<void>::failure("Total de commande invalide.");
    auto mappings = resolveMappings(order); if (!mappings) return mappings;
    if (config_.risk.require_valid_canadian_postal_code) {auto compliance=compliance_.validateAddress(order.shipping_address,config_.risk.block_po_boxes);if(!compliance){++counters_.rejected_events;return compliance;}}
    auto approved=risk_.approveOrder(order);if(!approved){++counters_.rejected_events;return approved;}
    auto admitted=admission_.admit(order,config_.app.dry_run||!config_.app.live_orders,true);if(!admitted){++counters_.rejected_events;return Result<void>::failure(admitted.error());}
    switch(admitted.value().decision){case shopify::ShopifyOrderAdmissionDecision::accepted:++counters_.orders_received;++counters_.accepted_events;counters_.estimated_profit_cad+=admitted.value().estimated_profit_cad;log_.info("orders","Commande Shopify "+order.shopify_order_id+" admise et mise en file fournisseur.");return Result<void>::success();case shopify::ShopifyOrderAdmissionDecision::duplicate:log_.debug("orders","Doublon Shopify ignoré: "+order.shopify_order_id);return Result<void>::success();case shopify::ShopifyOrderAdmissionDecision::manual_review:++counters_.orders_received;++counters_.accepted_events;log_.warning("orders","Commande Shopify "+order.shopify_order_id+" placée en révision manuelle.");return Result<void>::success();default:++counters_.rejected_events;return Result<void>::failure("Commande Shopify rejetée par le service d'admission.");}
}
'''
p.write_text(t[:start]+new+t[end:],encoding='utf-8')

# Autonomy header/cpp
replace('include/elit21/shopify/ShopifyAutonomyEngine.h','#include "elit21/shopify/ShopifyClient.h"\n','#include "elit21/shopify/ShopifyClient.h"\n#include "elit21/shopify/ShopifyOrderAdmissionService.h"\n')
replace('include/elit21/shopify/ShopifyAutonomyEngine.h','''    ShopifyAutonomyEngine(ShopifyClient& client,
                          Database& database,
                          ShopifyConfig config);
''','''    ShopifyAutonomyEngine(ShopifyClient& client,
                          Database& database,
                          ShopifyConfig config,
                          PricingConfig pricing = {},
                          InventoryConfig inventory = {},
                          RiskConfig risk = {});
''')
replace('include/elit21/shopify/ShopifyAutonomyEngine.h','''    ShopifyConfig config_;
''','''    ShopifyConfig config_;
    ShopifyOrderAdmissionService admission_;
''')
replace('src/elit21/shopify/ShopifyAutonomyEngine.cpp','''ShopifyAutonomyEngine::ShopifyAutonomyEngine(ShopifyClient& client,
                                             Database& database,
                                             ShopifyConfig config)
    : client_(client), database_(database), config_(std::move(config)) {}
''','''ShopifyAutonomyEngine::ShopifyAutonomyEngine(ShopifyClient& client,
                                             Database& database,
                                             ShopifyConfig config,
                                             PricingConfig pricing,
                                             InventoryConfig inventory,
                                             RiskConfig risk)
    : client_(client), database_(database), config_(std::move(config)),
      admission_(database_, std::move(pricing), std::move(inventory), std::move(risk)) {}
''')
p=ROOT/'src/elit21/shopify/ShopifyAutonomyEngine.cpp';t=p.read_text(encoding='utf-8');start=t.index('Result<int> ShopifyAutonomyEngine::ingestPaidOrders');end=t.index('\nResult<ShopifyAutonomyReport> ShopifyAutonomyEngine::run',start)
new=r'''Result<int> ShopifyAutonomyEngine::ingestPaidOrders(const std::vector<CustomerOrder>& orders,bool dry_run,ShopifyAutonomyReport& report) {int admitted_count=0;for(const auto&order:orders){auto admitted=admission_.admit(order,dry_run,true);if(!admitted){report.warnings.push_back("Shopify order "+order.shopify_order_id+" rejected: "+admitted.error());continue;}switch(admitted.value().decision){case shopify::ShopifyOrderAdmissionDecision::accepted:++admitted_count;++report.new_orders_persisted;if(admitted.value().supplier_task_enqueued)++report.tasks_enqueued;break;case shopify::ShopifyOrderAdmissionDecision::duplicate:++report.duplicate_orders;break;case shopify::ShopifyOrderAdmissionDecision::manual_review:++report.new_orders_persisted;if(admitted.value().review_task_enqueued)++report.tasks_enqueued;for(const auto&w:admitted.value().warnings)report.warnings.push_back(w);break;default:report.warnings.push_back("Shopify order rejected: "+order.shopify_order_id);break;}}return Result<int>::success(admitted_count);}
'''
p.write_text(t[:start]+new+t[end:],encoding='utf-8')
print('phase3 done')
