#include "elit21/platform/BusinessComponent.h"

#include "elit21/json/Json.h"
#include "elit21/util/StringUtil.h"

#include <algorithm>
#include <cmath>
#include <regex>
#include <sstream>

namespace elit21::platform {
namespace {

bool truthy(const std::optional<std::string>& value) {
    if (!value) return false;
    const auto lowered = util::lower(util::trim(*value));
    return lowered == "1" || lowered == "true" || lowered == "yes" || lowered == "on";
}

double clamp(double value, double low, double high) {
    return std::max(low, std::min(value, high));
}

bool validCanadianPostalCode(const std::string& value) {
    static const std::regex expression(
        R"(^[ABCEGHJ-NPRSTVXY]\d[ABCEGHJ-NPRSTV-Z][ -]?\d[ABCEGHJ-NPRSTV-Z]\d$)",
        std::regex::icase);
    return std::regex_match(util::trim(value), expression);
}

std::string normalizedCorrelation(const OperationContext& context, std::string_view component) {
    if (!context.correlation_id.empty()) return context.correlation_id;
    return "auto-" + std::to_string(stableHash(std::string(component) + context.payload));
}

} // namespace

BusinessComponent::BusinessComponent(std::string name, std::string role, BusinessComponentSpec spec)
    : ManagedComponent(std::move(name), std::move(role)), spec_(std::move(spec)) {}

OperationResult BusinessComponent::executeBusiness(const OperationContext& context) const {
    if (context.payload.size() > spec_.max_payload_bytes) {
        return OperationResult::failure("PAYLOAD_TOO_LARGE", "Payload exceeds the configured component ceiling");
    }

    Json document = Json::object();
    if (!context.payload.empty()) {
        auto parsed = Json::parse(context.payload);
        if (!parsed) {
            return OperationResult::failure("INVALID_JSON", "Business payload must be valid JSON: " + parsed.error());
        }
        document = parsed.take();
        if (!document.isObject() && !document.isArray()) {
            return OperationResult::failure("INVALID_PAYLOAD_TYPE", "Business payload must be a JSON object or array");
        }
    }

    if (document.isObject()) {
        for (const auto& field : spec_.required_fields) {
            if (!document.contains(field) || document.get(field).isNull()) {
                return OperationResult::failure("MISSING_FIELD", "Required field is missing: " + field);
            }
        }
    }

    if (spec_.mutating && !context.dry_run && spec_.requires_credentials && !truthy(setting("credentials_ready"))) {
        return OperationResult::failure(
            "CREDENTIALS_NOT_CONFIRMED",
            "Live mutation rejected until credentials_ready=true is configured explicitly");
    }

    auto result = OperationResult::ok(componentName() + " completed a validated " + spec_.operation + " operation");
    result.attributes["component"] = componentName();
    result.attributes["domain"] = spec_.domain;
    result.attributes["operation"] = spec_.operation;
    result.attributes["role"] = componentRole();
    result.attributes["mode"] = context.dry_run ? "dry_run" : "live";
    result.attributes["correlation_id"] = normalizedCorrelation(context, componentName());
    result.attributes["side_effect"] = spec_.mutating ? (context.dry_run ? "planned" : "authorized") : "none";
    result.metrics["payload_bytes"] = static_cast<double>(context.payload.size());
    result.metrics["payload_checksum"] = static_cast<double>(stableHash(context.payload) % 1000000000ULL);
    result.metrics["required_field_count"] = static_cast<double>(spec_.required_fields.size());

    Json output = Json::object();
    output.set("domain", spec_.domain);
    output.set("operation", spec_.operation);
    output.set("dry_run", context.dry_run);

    const auto domain = util::lower(spec_.domain);
    const auto operation = util::lower(spec_.operation);

    if (domain == "pricing") {
        const double supplier = document.getNumber("supplier_cost_cad", 0.0);
        const double shipping = document.getNumber("shipping_cad", 0.0);
        const double requested_markup = document.getNumber("markup_percent", 100.0);
        if (supplier < 0.0 || shipping < 0.0) {
            return OperationResult::failure("NEGATIVE_COST", "Supplier and shipping costs must be non-negative");
        }
        const double markup = std::max(100.0, requested_markup);
        const double before_shipping = supplier * (1.0 + markup / 100.0);
        const double final_price = std::round((before_shipping + shipping) * 100.0) / 100.0;
        output.set("markup_percent", markup);
        output.set("price_before_shipping_cad", before_shipping);
        output.set("final_price_cad", final_price);
        result.metrics["supplier_cost_cad"] = supplier;
        result.metrics["price_before_shipping_cad"] = before_shipping;
        result.metrics["final_price_cad"] = final_price;
        result.attributes["pricing_invariant"] = "supplier_cost_x2_then_shipping";
    } else if (domain == "inventory") {
        const int supplier_stock = document.getInt("supplier_stock", document.getInt("stock", 0));
        const int safety_buffer = std::max(0, document.getInt("safety_buffer", 2));
        const int available = std::max(0, supplier_stock - safety_buffer);
        output.set("supplier_stock", supplier_stock);
        output.set("safety_buffer", safety_buffer);
        output.set("available_to_sell", available);
        result.metrics["available_to_sell"] = available;
        result.attributes["oversell_guard"] = available == 0 ? "blocked" : "available";
    } else if (domain == "orders") {
        const double total = document.getNumber("order_total_cad", document.getNumber("total_cad", 0.0));
        const int quantity = document.getInt("quantity", document.getInt("line_count", 0));
        if (total < 0.0 || quantity < 0) {
            return OperationResult::failure("INVALID_ORDER", "Order totals and quantities cannot be negative");
        }
        output.set("order_total_cad", total);
        output.set("line_or_quantity_count", quantity);
        output.set("idempotency_hash", static_cast<double>(stableHash(result.attributes["correlation_id"])));
        result.metrics["order_total_cad"] = total;
        result.metrics["line_or_quantity_count"] = quantity;
        result.attributes["idempotency"] = "enforced_by_correlation";
    } else if (domain == "fulfillment") {
        const auto tracking = document.getString("tracking_number");
        const auto carrier = document.getString("carrier");
        output.set("tracking_number_present", !tracking.empty());
        output.set("carrier", carrier);
        result.metrics["tracking_length"] = static_cast<double>(tracking.size());
        result.attributes["fulfillment_state"] = tracking.empty() ? "awaiting_tracking" : "trackable";
    } else if (domain == "compliance") {
        const auto country = document.getString("country_code", "CA");
        const auto currency = document.getString("currency", "CAD");
        const auto postal = document.getString("postal_code");
        if (country != "CA") return OperationResult::failure("COUNTRY_NOT_ALLOWED", "Only Canadian destinations are allowed");
        if (currency != "CAD") return OperationResult::failure("CURRENCY_NOT_ALLOWED", "Only CAD pricing is allowed");
        if (!postal.empty() && !validCanadianPostalCode(postal)) {
            return OperationResult::failure("INVALID_POSTAL_CODE", "Canadian postal code format is invalid");
        }
        output.set("country_code", country);
        output.set("currency", currency);
        output.set("postal_code_valid", postal.empty() || validCanadianPostalCode(postal));
        result.attributes["market"] = "Canada";
    } else if (domain == "risk") {
        const double total = document.getNumber("order_total_cad", 0.0);
        const double ceiling = document.getNumber("maximum_order_value_cad", 750.0);
        const bool accepted = total <= ceiling;
        output.set("accepted", accepted);
        output.set("risk_ratio", ceiling > 0.0 ? total / ceiling : 1.0);
        result.metrics["risk_ratio"] = ceiling > 0.0 ? total / ceiling : 1.0;
        if (!accepted) return OperationResult::failure("RISK_LIMIT_EXCEEDED", "Order exceeds the configured risk ceiling");
    } else if (domain == "finance") {
        const double revenue = document.getNumber("revenue_cad", 0.0);
        const double supplier = document.getNumber("supplier_cost_cad", 0.0);
        const double shipping = document.getNumber("shipping_cad", 0.0);
        const double fees = document.getNumber("fees_cad", 0.0);
        const double profit = revenue - supplier - shipping - fees;
        output.set("net_profit_cad", profit);
        output.set("margin_percent", revenue > 0.0 ? (profit / revenue) * 100.0 : 0.0);
        result.metrics["net_profit_cad"] = profit;
    } else if (domain == "mapping") {
        const auto source = document.getString("source_id");
        const auto target = document.getString("target_id");
        output.set("mapping_complete", !source.empty() && !target.empty());
        output.set("mapping_key", source + "->" + target);
        result.attributes["mapping_state"] = (!source.empty() && !target.empty()) ? "complete" : "incomplete";
    } else if (domain == "security") {
        const auto lowered = util::lower(context.payload);
        const bool contains_secret = lowered.find("access_token") != std::string::npos ||
                                     lowered.find("password") != std::string::npos ||
                                     lowered.find("app_secret") != std::string::npos;
        output.set("sensitive_material_detected", contains_secret);
        result.metrics["sensitive_marker_count"] = contains_secret ? 1.0 : 0.0;
        result.attributes["redaction_required"] = contains_secret ? "true" : "false";
    } else if (domain == "shopify") {
        const auto query = document.getString("query");
        if (!query.empty() && query.find("query") == std::string::npos && query.find("mutation") == std::string::npos) {
            return OperationResult::failure("INVALID_GRAPHQL_DOCUMENT", "Shopify document must contain query or mutation");
        }
        output.set("graphql_document_present", !query.empty());
        output.set("api_version", document.getString("api_version", "2026-07"));
        result.attributes["transport"] = "Shopify Admin GraphQL";
    } else if (domain == "aliexpress") {
        const auto method = document.getString("method");
        if (!method.empty() && method.rfind("aliexpress.", 0) != 0 && method.rfind("auth/", 0) != 0) {
            return OperationResult::failure("INVALID_ALIEXPRESS_METHOD", "AliExpress method name is not recognized");
        }
        output.set("method", method);
        output.set("ship_to_country", document.getString("ship_to_country", "CA"));
        output.set("target_currency", document.getString("target_currency", "CAD"));
        result.attributes["supplier_market"] = "CA/CAD";
    } else if (domain == "network") {
        const auto url = document.getString("url");
        if (!url.empty() && url.rfind("https://", 0) != 0 && url.rfind("http://127.0.0.1", 0) != 0 && url.rfind("http://localhost", 0) != 0) {
            return OperationResult::failure("INSECURE_URL", "Only HTTPS or local loopback HTTP endpoints are allowed");
        }
        output.set("url", url);
        result.attributes["tls_policy"] = "https_required_except_loopback";
    } else if (domain == "scheduler") {
        const int interval = std::max(1, document.getInt("interval_seconds", 60));
        output.set("interval_seconds", interval);
        result.metrics["interval_seconds"] = interval;
    } else if (domain == "terminal") {
        const double progress = clamp(document.getNumber("progress", 0.0), 0.0, 1.0);
        output.set("progress", progress);
        output.set("progress_percent", progress * 100.0);
        result.metrics["progress_percent"] = progress * 100.0;
        result.attributes["render_mode"] = "fixed_position";
    } else if (domain == "analytics" || domain == "reports") {
        const double accepted = document.getNumber("accepted", 0.0);
        const double rejected = document.getNumber("rejected", 0.0);
        const double total = accepted + rejected;
        output.set("acceptance_rate", total > 0.0 ? accepted / total : 0.0);
        result.metrics["acceptance_rate"] = total > 0.0 ? accepted / total : 0.0;
    } else if (domain == "workflow" || domain == "automation") {
        const auto state = document.getString("state", "pending");
        output.set("state", state);
        output.set("transition_allowed", state != "failed" || context.dry_run);
        result.attributes["workflow_state"] = state;
    } else if (domain == "storage" || domain == "repositories") {
        const auto entity_id = document.getString("entity_id");
        output.set("entity_id", entity_id);
        output.set("write_planned", spec_.mutating && context.dry_run);
        result.attributes["persistence"] = spec_.mutating ? "transactional_write" : "consistent_read";
    } else if (domain == "customer_service" || domain == "customers") {
        const auto language = document.getString("language", "fr-CA");
        const auto email = document.getString("email");
        const auto consent = document.getBool("consent", false);
        if (!email.empty() && email.find('@') == std::string::npos) {
            return OperationResult::failure("INVALID_EMAIL", "Customer email address is invalid");
        }
        output.set("language", language);
        output.set("contact_allowed", email.empty() || consent || operation.find("delete") != std::string::npos);
        result.attributes["privacy_mode"] = "minimal_data";
        result.attributes["language_policy"] = language == "fr-CA" || language == "en-CA" ? "canadian" : "fallback";
    } else if (domain == "application") {
        const auto state = util::lower(document.getString("state", "created"));
        const auto requested = util::lower(document.getString("requested_state", state));
        static const std::vector<std::string> states{"created", "configured", "starting", "running", "stopping", "stopped", "faulted"};
        if (std::find(states.begin(), states.end(), state) == states.end() ||
            std::find(states.begin(), states.end(), requested) == states.end()) {
            return OperationResult::failure("INVALID_LIFECYCLE_STATE", "Application lifecycle state is invalid");
        }
        const bool transition = state == requested ||
            (state == "created" && requested == "configured") ||
            (state == "configured" && requested == "starting") ||
            (state == "starting" && requested == "running") ||
            (state == "running" && requested == "stopping") ||
            (state == "stopping" && requested == "stopped") || requested == "faulted";
        output.set("state", state);
        output.set("requested_state", requested);
        output.set("transition_allowed", transition);
        if (!transition) return OperationResult::failure("INVALID_STATE_TRANSITION", "Application state transition is not allowed");
    } else if (domain == "configuration") {
        const auto country = document.getString("country", document.getString("country_code", "CA"));
        const auto currency = document.getString("currency", "CAD");
        const double markup = document.getNumber("markup_percent_before_shipping", 100.0);
        const bool live = document.getBool("live_orders", false);
        const bool credentials = document.getBool("credentials_ready", false);
        if (country != "CA" || currency != "CAD" || markup < 100.0) {
            return OperationResult::failure("INVALID_CONFIGURATION", "Canada/CAD and a minimum 100 percent markup are mandatory");
        }
        if (live && !credentials) return OperationResult::failure("LIVE_MODE_NOT_AUTHORIZED", "Live mode requires confirmed credentials");
        output.set("country", country);
        output.set("currency", currency);
        output.set("markup_percent_before_shipping", markup);
        output.set("live_mode_authorized", !live || credentials);
        result.attributes["configuration_policy"] = "validated_and_fail_closed";
    } else if (domain == "concurrency") {
        const int workers = std::max(1, document.getInt("worker_threads", 1));
        const int queue_depth = std::max(0, document.getInt("queue_depth", 0));
        const int capacity = std::max(1, document.getInt("capacity", 1000));
        const bool cancelled = document.getBool("cancelled", false);
        if (workers > 64) return OperationResult::failure("WORKER_LIMIT_EXCEEDED", "Worker thread count exceeds 64");
        if (queue_depth > capacity) return OperationResult::failure("QUEUE_CAPACITY_EXCEEDED", "Queue depth exceeds capacity");
        output.set("worker_threads", workers);
        output.set("queue_depth", queue_depth);
        output.set("capacity", capacity);
        output.set("cancelled", cancelled);
        result.metrics["queue_utilization"] = static_cast<double>(queue_depth) / capacity;
        result.attributes["concurrency_state"] = cancelled ? "cancelled" : "runnable";
    } else if (domain == "messaging") {
        const auto message_id = document.getString("message_id", result.attributes["correlation_id"]);
        const auto message_type = document.getString("message_type", operation);
        const int delivery_attempt = std::max(0, document.getInt("delivery_attempt", 0));
        if (message_id.empty() || message_type.empty()) return OperationResult::failure("INVALID_MESSAGE", "Message identity and type are required");
        output.set("message_id", message_id);
        output.set("message_type", message_type);
        output.set("delivery_attempt", delivery_attempt);
        output.set("deduplication_key", static_cast<double>(stableHash(message_id)));
        result.attributes["delivery_semantics"] = "at_least_once_with_idempotency";
    } else if (domain == "resilience") {
        const auto component = util::lower(componentName());
        const int attempt = std::max(0, document.getInt("attempt", 0));
        const int maximum = std::max(1, document.getInt("max_attempts", 5));
        const int failures = std::max(0, document.getInt("failure_count", 0));
        const int threshold = std::max(1, document.getInt("failure_threshold", 5));
        const double base_delay = std::max(0.001, document.getNumber("base_delay_seconds", 1.0));
        const double maximum_delay = std::max(base_delay, document.getNumber("maximum_delay_seconds", 300.0));
        const double delay = std::min(maximum_delay, base_delay * std::pow(2.0, std::min(attempt, 20)));
        output.set("attempt", attempt);
        output.set("max_attempts", maximum);
        output.set("retry_allowed", attempt < maximum);
        output.set("backoff_seconds", delay);
        output.set("circuit_open", failures >= threshold);
        result.metrics["backoff_seconds"] = delay;
        if (component.find("circuit") != std::string::npos && failures >= threshold && !context.dry_run) {
            return OperationResult::failure("CIRCUIT_OPEN", "Circuit breaker is open");
        }
        if (component.find("retry") != std::string::npos && attempt >= maximum) {
            return OperationResult::failure("RETRY_EXHAUSTED", "Maximum retry attempts reached");
        }
        result.attributes["resilience_policy"] = "bounded_exponential_backoff";
    } else if (domain == "serialization") {
        const auto format = util::lower(document.getString("format", "json"));
        if (format != "json" && format != "csv" && format != "query_string") {
            return OperationResult::failure("UNSUPPORTED_FORMAT", "Serialization format is not supported");
        }
        output.set("format", format);
        output.set("valid", true);
        output.set("record_count", document.isArray() ? static_cast<int>(document.size()) : 1);
        result.attributes["encoding"] = "UTF-8";
    } else if (domain == "core" || domain == "domain") {
        const auto currency = document.getString("currency", "CAD");
        const auto country = document.getString("country_code", "CA");
        const double amount = document.getNumber("amount", 0.0);
        const int quantity = document.getInt("quantity", 0);
        if (amount < 0.0 || quantity < 0) return OperationResult::failure("NEGATIVE_VALUE", "Domain values cannot be negative");
        if (!currency.empty() && currency != "CAD") return OperationResult::failure("INVALID_CURRENCY", "Domain currency must be CAD");
        if (!country.empty() && country != "CA") return OperationResult::failure("INVALID_COUNTRY", "Domain country must be CA");
        output.set("currency", currency);
        output.set("country_code", country);
        output.set("amount", amount);
        output.set("quantity", quantity);
        output.set("normalized_identifier", document.getString("id", result.attributes["correlation_id"]));
        result.attributes["value_object_state"] = "validated";
    } else if (domain == "catalog") {
        const auto product_id = document.getString("product_id");
        const auto title = util::trim(document.getString("title"));
        const int stock = std::max(0, document.getInt("stock", 0));
        const int image_count = std::max(0, document.getInt("image_count", 0));
        const bool eligible = !product_id.empty() && !title.empty() && stock > 0;
        output.set("product_id", product_id);
        output.set("normalized_title", title);
        output.set("stock", stock);
        output.set("image_count", image_count);
        output.set("publish_eligible", eligible);
        result.attributes["catalog_state"] = eligible ? "ready" : "incomplete";
    } else if (domain == "sourcing") {
        const double rating = clamp(document.getNumber("rating", 0.0), 0.0, 5.0);
        const double seller = clamp(document.getNumber("seller_score", 0.0), 0.0, 100.0);
        const int orders = std::max(0, document.getInt("orders", 0));
        const int delivery_days = std::max(0, document.getInt("delivery_days", 0));
        const double score = rating / 5.0 * 35.0 + seller / 100.0 * 35.0 +
            std::min(20.0, std::log10(static_cast<double>(orders) + 1.0) * 6.0) +
            (delivery_days > 0 ? std::max(0.0, 10.0 - delivery_days / 6.0) : 0.0);
        output.set("score", score);
        output.set("accepted", score >= document.getNumber("minimum_score", 70.0));
        result.metrics["sourcing_score"] = score;
        result.attributes["supplier_evaluation"] = "rating_seller_orders_delivery";
    } else if (domain == "returns") {
        const double paid = document.getNumber("paid_cad", document.getNumber("amount_cad", 0.0));
        const double requested = document.getNumber("requested_refund_cad", paid);
        const int days = std::max(0, document.getInt("days_since_delivery", 0));
        if (paid < 0.0 || requested < 0.0 || requested > paid) {
            return OperationResult::failure("INVALID_REFUND_AMOUNT", "Refund amount exceeds the paid amount");
        }
        const bool eligible = days <= std::max(1, document.getInt("return_window_days", 30));
        output.set("refund_cad", requested);
        output.set("eligible", eligible);
        output.set("days_since_delivery", days);
        result.attributes["return_decision"] = eligible ? "eligible" : "outside_window";
    } else if (domain == "observability") {
        const auto level = util::lower(document.getString("level", "info"));
        const auto health = util::lower(document.getString("health", "healthy"));
        const double latency = std::max(0.0, document.getNumber("latency_ms", 0.0));
        output.set("level", level);
        output.set("health", health);
        output.set("latency_ms", latency);
        output.set("alert", level == "critical" || level == "error" || health != "healthy");
        result.metrics["latency_ms"] = latency;
        result.attributes["telemetry"] = "structured";
    } else {
        output.set("validated", true);
        output.set("operation_class", operation);
        output.set("component", componentName());
        result.attributes["shared_engine"] = "domain_aware";
    }

    result.attributes["output_json"] = output.dump();
    return result;
}

} // namespace elit21::platform
