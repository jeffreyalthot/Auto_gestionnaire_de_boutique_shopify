#include "elit21/shopify/ShopifyApiContractAuditor.h"

#include "elit21/shopify/ShopifyInventoryWritePlanner.h"
#include "elit21/storage/Database.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace elit21::shopify {
namespace {

std::string readText(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    std::ostringstream output;
    output << input.rdbuf();
    return output.str();
}

void add(ShopifyApiContractReport& report,
         std::string severity,
         const std::filesystem::path& path,
         std::string rule,
         std::string message) {
    ShopifyApiContractFinding finding;
    finding.severity = std::move(severity);
    finding.path = path.generic_string();
    finding.rule = std::move(rule);
    finding.message = std::move(message);
    if (finding.severity == "error") ++report.errors;
    else ++report.warnings;
    report.findings.push_back(std::move(finding));
}

bool isSource(const std::filesystem::path& path) {
    const auto extension = path.extension().string();
    return extension == ".cpp" || extension == ".h" || extension == ".hpp";
}

} // namespace

Json ShopifyApiContractFinding::toJson() const {
    Json output = Json::object();
    output.set("severity", severity);
    output.set("path", path);
    output.set("rule", rule);
    output.set("message", message);
    return output;
}

Json ShopifyApiContractReport::toJson() const {
    Json output = Json::object();
    output.set("score", score);
    output.set("passed", passed);
    output.set("files_scanned", static_cast<std::int64_t>(files_scanned));
    output.set("errors", static_cast<std::int64_t>(errors));
    output.set("warnings", static_cast<std::int64_t>(warnings));
    Json list = Json::array();
    for (const auto& finding : findings) list.push(finding.toJson());
    output.set("findings", list);
    return output;
}

std::string ShopifyApiContractReport::markdown() const {
    std::ostringstream output;
    output << "# Shopify API Contract Audit\n\n"
           << "- Score: **" << score << "/100**\n"
           << "- Passed: **" << (passed ? "yes" : "no") << "**\n"
           << "- Files scanned: **" << files_scanned << "**\n"
           << "- Errors: **" << errors << "**\n"
           << "- Warnings: **" << warnings << "**\n\n";
    for (const auto& finding : findings) {
        output << "- **" << finding.severity << "** `" << finding.rule << "` — `"
               << finding.path << "`: " << finding.message << "\n";
    }
    return output.str();
}

Result<ShopifyApiContractReport> ShopifyApiContractAuditor::audit(
    const std::filesystem::path& project_root,
    Database* database) {
    if (!std::filesystem::is_directory(project_root)) {
        return Result<ShopifyApiContractReport>::failure("Project root is not a directory");
    }

    ShopifyApiContractReport report;
    const std::vector<std::filesystem::path> roots{
        project_root / "include" / "elit21" / "shopify",
        project_root / "src" / "elit21" / "shopify"};

    bool inventory_document_found = false;
    bool product_set_document_found = false;
    for (const auto& scan_root : roots) {
        if (!std::filesystem::exists(scan_root)) continue;
        for (const auto& item : std::filesystem::recursive_directory_iterator(scan_root)) {
            if (!item.is_regular_file() || !isSource(item.path())) continue;
            ++report.files_scanned;
            const auto text = readText(item.path());
            const auto relative = std::filesystem::relative(item.path(), project_root);

            const bool auditor_source = relative.filename() == "ShopifyApiContractAuditor.cpp";
            if (!auditor_source && text.find("productSet(") != std::string::npos &&
                text.find("ProductSetInput") != std::string::npos) {
                product_set_document_found = true;
            }
            if (!auditor_source &&
                (text.find("input.set(\"ignoreCompareQuantity\"") != std::string::npos ||
                 text.find("set(\"compareQuantity\"") != std::string::npos ||
                 text.find("ignoreCompareQuantity:") != std::string::npos ||
                 text.find("compareQuantity:") != std::string::npos)) {
                add(report, "error", relative, "SHOPIFY-INV-2026-04",
                    "legacy compareQuantity/ignoreCompareQuantity field detected");
            }
            const bool inventory_document = relative.filename() == "InventoryMutations.cpp" ||
                                            relative.filename() == "ShopifyInventoryWritePlanner.cpp";
            if (inventory_document && text.find("inventorySetQuantities") != std::string::npos) {
                inventory_document_found = true;
                if (text.find("@idempotent") == std::string::npos) {
                    add(report, "error", relative, "SHOPIFY-IDEMPOTENCY-2026-04",
                        "inventorySetQuantities mutation must use @idempotent");
                }
            }
            if (relative.filename() == "ShopifyInventoryWritePlanner.cpp" &&
                text.find("changeFromQuantity") == std::string::npos) {
                add(report, "error", relative, "SHOPIFY-INV-CAS",
                    "inventory planner must emit changeFromQuantity");
            }
            if (item.path().extension() == ".cpp" &&
                relative.generic_string().find("/mutations/") != std::string::npos &&
                text.find("R\"graphql(") != std::string::npos &&
                text.find("mutation") != std::string::npos &&
                text.find("userErrors") == std::string::npos &&
                text.find("UserErrors") == std::string::npos &&
                text.find("bulkOperationCancel") == std::string::npos) {
                add(report, "warning", relative, "SHOPIFY-USER-ERRORS",
                    "mutation document does not request userErrors");
            }
            if (!auditor_source &&
                (text.find("/admin/api/2025-") != std::string::npos ||
                 text.find("/admin/api/2024-") != std::string::npos)) {
                add(report, "error", relative, "SHOPIFY-API-VERSION",
                    "outdated hard-coded Shopify API version detected");
            }
        }
    }

    if (!product_set_document_found) {
        add(report, "error", "src/elit21/shopify", "SHOPIFY-PRODUCT-SET",
            "no productSet external catalog synchronization document was found");
    }
    if (!inventory_document_found) {
        add(report, "error", "src/elit21/shopify", "SHOPIFY-INV-MISSING",
            "no inventorySetQuantities implementation was found");
    }
    auto planner_audit = ShopifyInventoryWritePlanner::auditMutationDocument(
        ShopifyInventoryWritePlanner::mutationDocument());
    if (!planner_audit) {
        add(report, "error", "ShopifyInventoryWritePlanner", "SHOPIFY-INV-DOCUMENT",
            planner_audit.error());
    }

    const auto config = project_root / "config" / "app.json";
    if (std::filesystem::exists(config)) {
        const auto text = readText(config);
        if (text.find("2026-07") == std::string::npos) {
            add(report, "warning", "config/app.json", "SHOPIFY-CONFIG-VERSION",
                "configured Shopify API version is not visibly locked to 2026-07");
        }
    }

    report.score = std::max(0, 100 - static_cast<int>(report.errors) * 20 -
                                  static_cast<int>(report.warnings) * 4);
    report.passed = report.errors == 0;
    if (database != nullptr) {
        auto stored = database->recordShopifyContractAudit(
            report.score, report.passed, report.toJson().dump());
        if (!stored) return Result<ShopifyApiContractReport>::failure(stored.error());
    }
    return Result<ShopifyApiContractReport>::success(std::move(report));
}

} // namespace elit21::shopify
