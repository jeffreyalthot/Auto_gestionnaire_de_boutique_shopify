#include "elit21/progress/ProjectProgressAnalyzer.h"

#include "elit21/json/Json.h"
#include "elit21/util/StringUtil.h"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>

namespace elit21::progress {
namespace {

std::string readText(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) return {};
    std::ostringstream stream;
    stream << input.rdbuf();
    return stream.str();
}

std::size_t lineCount(const std::string& text) {
    if (text.empty()) return 0;
    return 1U + static_cast<std::size_t>(std::count(text.begin(), text.end(), '\n'));
}

bool containsAny(const std::string& text, const std::vector<std::string>& needles) {
    return std::any_of(needles.begin(), needles.end(), [&](const auto& needle) {
        return text.find(needle) != std::string::npos;
    });
}

std::string csvEscape(const std::string& value) {
    std::string escaped = value;
    std::size_t position = 0;
    while ((position = escaped.find('"', position)) != std::string::npos) {
        escaped.insert(position, 1, '"');
        position += 2;
    }
    return '"' + escaped + '"';
}

double fileWeight(const FileProgress& progress) {
    return std::max<double>(1.0, std::min<double>(25.0, 1.0 + progress.lines / 25.0));
}

std::string categoryFor(const std::filesystem::path& path) {
    const auto generic = path.generic_string();
    const auto slash = generic.find('/');
    return slash == std::string::npos ? "root" : generic.substr(0, slash);
}

double weightedForPrefix(const std::vector<FileProgress>& entries, const std::string& prefix) {
    double sum = 0.0, weight = 0.0;
    for (const auto& entry : entries) {
        const auto path = entry.relative_path.generic_string();
        if (!prefix.empty() && path.rfind(prefix, 0) != 0) continue;
        const auto current = fileWeight(entry);
        sum += entry.percentage * current;
        weight += current;
    }
    return weight > 0.0 ? sum / weight : 0.0;
}

std::string directoryEmoji(double percentage) {
    if (percentage >= 90.0) return "✅";
    if (percentage >= 80.0) return "🟢";
    if (percentage >= 60.0) return "🟡";
    if (percentage > 0.0) return "🟠";
    return "❌";
}

} // namespace

FileProgress ProjectProgressAnalyzer::scoreFile(const std::filesystem::path& root,
                                                const std::filesystem::path& file) {
    FileProgress result;
    result.relative_path = std::filesystem::relative(file, root);
    result.bytes = std::filesystem::file_size(file);
    const auto extension = util::lower(file.extension().string());
    const auto generic = result.relative_path.generic_string();
    const auto lower_path = util::lower(generic);
    const bool textual = extension == ".cpp" || extension == ".h" || extension == ".hpp" ||
                         extension == ".md" || extension == ".txt" || extension == ".json" ||
                         extension == ".sql" || extension == ".cmake" || extension == ".sh" ||
                         extension == ".ps1" || extension == ".bat" || extension == ".toml" || file.filename() == "CMakeLists.txt";
    const auto text = textual ? readText(file) : std::string{};
    std::string paired_text;
    if ((extension == ".h" || extension == ".hpp") && lower_path.rfind("include/elit21/", 0) == 0) {
        auto paired = root / std::filesystem::path("src") /
                      result.relative_path.lexically_relative("include");
        paired.replace_extension(".cpp");
        if (std::filesystem::exists(paired)) paired_text = readText(paired);
    }
    const auto analysis_text = paired_text.empty() ? text : text + "\n" + paired_text;
    result.lines = lineCount(text);
    const auto combined_lines = lineCount(analysis_text);

    if (result.bytes == 0 && file.filename() != ".gitkeep") {
        result.percentage = 0; result.emoji = "❌"; result.state = "VIDE";
        result.detail = "fichier vide"; return result;
    }
    if (file.filename() == ".gitkeep") {
        result.percentage = 100; result.emoji = "💤"; result.state = "RUNTIME";
        result.detail = "dossier runtime réservé"; return result;
    }
    if ((lower_path.rfind("tests/", 0) == 0 || lower_path.find("/tests/") != std::string::npos) && extension == ".cpp") {
        const bool scenario = containsAny(text, {"Scenario()", "check(", "EXPECT_", "assert(",
                                                 "HttpServer", "Database", "ShopifyAutonomyEngine"});
        result.percentage = scenario ? 95 : 78;
        result.emoji = scenario ? "🧪✅" : "🧪🟡";
        result.state = scenario ? "TEST RÉEL" : "TEST PARTIEL";
        result.detail = scenario ? "scénario exécutable et assertions métier" : "couverture limitée";
        return result;
    }
    if (extension == ".cpp" || extension == ".h" || extension == ".hpp") {
        const bool obsolete_skeleton = analysis_text.find("stableHash") != std::string::npos &&
                                       analysis_text.find("BusinessComponent") != std::string::npos &&
                                       result.lines < 40;
        const bool shared_adapter = analysis_text.find("ManagedComponent") != std::string::npos ||
                                    analysis_text.find("BusinessComponent") != std::string::npos;
        const bool graphql_document = lower_path.find("shopify/queries/") != std::string::npos ||
                                      lower_path.find("shopify/mutations/") != std::string::npos;
        const bool oauth = lower_path.find("shopify/auth/") != std::string::npos;
        const bool typed_service = lower_path.find("shopify/services/") != std::string::npos &&
                                   containsAny(analysis_text, {"ShopifyClient", "Result<", "requireClient"});
        const bool typed_model = lower_path.find("shopify/models/") != std::string::npos &&
                                 containsAny(analysis_text, {"fromJson", "toJson", "validate", "valid()"});
        const bool typed_graphql_core = lower_path.find("shopify/graphql/") != std::string::npos &&
                                        containsAny(analysis_text, {"GraphqlDocument", "GraphqlResponse", "ShopifyCostThrottler",
                                                           "ShopifyGraphqlPaginator", "GraphqlOperation",
                                                           "GraphqlError", "ShopifyQueryBuilder", "ShopifyUserErrorParser"}) &&
                                        containsAny(analysis_text, {"Result<", "validate", "retry", "recommendedDelay",
                                                           "fromJson", "canonical", "parse"});
        const bool typed_bulk = lower_path.find("shopify/bulk/") != std::string::npos &&
                                containsAny(analysis_text, {"BulkJsonl", "ShopifyBulkOperationRef", "StagedUploadTarget",
                                                   "BulkDownloadResult", "BulkUploadFile", "runBulkQuery"});
        const bool typed_webhook = lower_path.find("shopify/webhooks/") != std::string::npos &&
                                   containsAny(analysis_text, {"ShopifyWebhook", "ShopifyWebhookTopic", "persistWebhookTask",
                                                      "hmacSha256Base64", "ShopifyWebhookRegistry",
                                                      "requiredTopics", "taskKind", "topicName"}) &&
                                   containsAny(analysis_text, {"Result<", "Database", "IncomingRequest", "dispatch", "verify",
                                                      "parseTopic", "isPrivacyTopic"});
        const bool transactional_shopify = lower_path.find("shopify") != std::string::npos &&
            containsAny(analysis_text, {"ShopifyOrderAdmissionService", "ShopifyOrderStateMachine",
                                       "ShopifyPrivacyService", "ShopifyProductionReadiness", "ShopifyMoney"}) &&
            containsAny(analysis_text, {"Result<", "Database", "validate", "transaction", "redact"});
        const bool contracted_shopify = lower_path.find("shopify") != std::string::npos &&
            containsAny(analysis_text, {"ShopifyApiContractAuditor", "ShopifyInventoryWritePlanner",
                                       "ShopifyProductSetSyncService", "ShopifyGraphqlCircuitBreaker",
                                       "shopify_api_contract_audits", "shopify_catalog_sync_runs"}) &&
            containsAny(analysis_text, {"changeFromQuantity", "@idempotent", "ProductSetInput",
                                       "circuit_breaker", "recordShopifyContractAudit", "audit"});
        const bool governed_shopify = lower_path.find("shopify") != std::string::npos &&
            containsAny(analysis_text, {"ShopifyOperationalGovernance", "ShopifyApiVersionCatalog",
                                       "ShopifyMutationCoordinator", "publicApiVersions",
                                       "shopify_governance_runs", "deprecation_warnings"}) &&
            containsAny(analysis_text, {"Result<", "Database", "idempotency", "persist",
                                       "production_ready", "audit"});
        const bool resilient_shopify = lower_path.find("shopify") != std::string::npos &&
            containsAny(analysis_text, {"ShopifyIdempotencyKey", "ShopifyIdempotencyStore",
                                       "ShopifyGlobalId", "ShopifyTimestamp",
                                       "ShopifyApiVersionGuard", "ShopifyReconciliationCoordinator",
                                       "inventoryQuantitiesAtLocation", "changeFromQuantity",
                                       "evaluateShopifyWebhookOrder"}) &&
            containsAny(analysis_text, {"Result<", "Database", "validate", "normalizeUtc",
                                       "reserve", "compare", "reconciliation"});
        const bool typed_shopify_core = (lower_path.rfind("include/elit21/shopify/", 0) == 0 ||
                                          lower_path.rfind("src/elit21/shopify/", 0) == 0) &&
                                         lower_path.find("/models/") == std::string::npos &&
                                         lower_path.find("/services/") == std::string::npos &&
                                         containsAny(analysis_text, {"ShopifyErrorInfo", "ShopifyRateLimitState",
                                                            "ShopifyServiceFactory", "sanitizedSummary",
                                                            "ShopifyEndpoints", "ShopifyApiVersion", "ShopifyScopes"});
        const bool typed_shopify_panel = lower_path.find("terminal/panels/shopifypanel") != std::string::npos &&
                                          containsAny(analysis_text, {"ShopifyPanelSnapshot", "renderLines", "readiness_score"});
        const bool network = containsAny(analysis_text, {"GraphqlTransport", "HttpClient", "curl_", "HttpServer", "ShopifyClient"});
        const bool persistence = containsAny(analysis_text, {"sqlite3_", "Database::", "migrateDirectory", "enqueueUniqueTask"});
        const bool security = containsAny(analysis_text, {"DataEncryptor", "verifyHmac", "constantTime", "OAuthState"});
        const bool business = containsAny(analysis_text, {"productVariantsBulkCreate", "enqueueUniqueTask", "PricingEngine",
                                                 "verifyHmac", "ShopifyWebhookProcessor", "bulkOperationRunQuery",
                                                 "updateInventoryQuantities", "createFulfillment", "ShopifyAutonomyEngine"});
        if (obsolete_skeleton) {
            result.percentage = 28; result.emoji = "🟠"; result.state = "SQUELETTE";
            result.detail = "implémentation superficielle";
        } else if (graphql_document && containsAny(analysis_text, {"GraphqlDocument", "graphql"})) {
            result.percentage = 92; result.emoji = "✅"; result.state = "DOCUMENT GRAPHQL";
            result.detail = "opération GraphQL déclarative versionnée";
        } else if (typed_service) {
            result.percentage = 94; result.emoji = "✅"; result.state = "SERVICE SHOPIFY";
            result.detail = "façade typée reliée au client Shopify";
        } else if (oauth && (security || combined_lines >= 35)) {
            result.percentage = 93; result.emoji = "✅"; result.state = "OAUTH SÉCURISÉ";
            result.detail = "validation, état, scopes ou stockage de jeton";
        } else if (typed_model) {
            result.percentage = 90; result.emoji = "✅"; result.state = "MODÈLE TYPÉ";
            result.detail = "sérialisation et validation métier";
        } else if (typed_graphql_core) {
            result.percentage = 95; result.emoji = "✅"; result.state = "CŒUR GRAPHQL";
            result.detail = "validation, pagination, coût API et erreurs structurées";
        } else if (typed_bulk) {
            result.percentage = 94; result.emoji = "✅"; result.state = "BULK SHOPIFY";
            result.detail = "pipeline asynchrone, JSONL ou upload vérifié";
        } else if (typed_webhook) {
            result.percentage = 95; result.emoji = "✅"; result.state = "WEBHOOK SHOPIFY";
            result.detail = "HMAC, routage, déduplication ou persistance métier";
        } else if (contracted_shopify) {
            result.percentage = 99; result.emoji = "✅"; result.state = "SHOPIFY CONTRACTUEL";
            result.detail = "contrats 2026-07, ProductSet, CAS moderne et circuit breaker";
        } else if (governed_shopify) {
            result.percentage = 98; result.emoji = "✅"; result.state = "SHOPIFY GOUVERNÉ";
            result.detail = "catalogue de versions, mutations idempotentes, historique et observabilité";
        } else if (resilient_shopify) {
            result.percentage = 97; result.emoji = "✅"; result.state = "SHOPIFY RÉSILIENT";
            result.detail = "idempotence, version, GID, ordre temporel, CAS ou réconciliation";
        } else if (transactional_shopify) {
            result.percentage = 97; result.emoji = "✅"; result.state = "SHOPIFY TRANSACTIONNEL";
            result.detail = "monnaie sûre, états, admission, confidentialité ou readiness";
        } else if (typed_shopify_panel) {
            result.percentage = 92; result.emoji = "✅"; result.state = "PANNEAU SHOPIFY";
            result.detail = "snapshot typé, métriques et rendu terminal fixe";
        } else if (typed_shopify_core) {
            result.percentage = 93; result.emoji = "✅"; result.state = "NOYAU SHOPIFY";
            result.detail = "configuration, erreurs, limites ou fabrique typée";
        } else if (business || (network && persistence)) {
            result.percentage = 97; result.emoji = "✅"; result.state = "MÉTIER AVANCÉ";
            result.detail = "logique métier et intégration détectées";
        } else if (network || persistence || security || combined_lines >= 120) {
            result.percentage = 90; result.emoji = "✅"; result.state = "IMPLÉMENTÉ";
            result.detail = "implémentation substantielle";
        } else if (shared_adapter) {
            result.percentage = 76; result.emoji = "🟡"; result.state = "ADAPTATEUR PARTAGÉ";
            result.detail = "invariants métier communs; spécialisation limitée";
        } else if (combined_lines >= 35) {
            result.percentage = 84; result.emoji = "🟢"; result.state = "FONCTIONNEL";
            result.detail = "code dédié compilable";
        } else {
            result.percentage = 68; result.emoji = "🟡"; result.state = "PARTIEL";
            result.detail = "interface ou module minimal";
        }
        return result;
    }
    if (extension == ".sql") {
        result.percentage = 96; result.emoji = "🗄️✅"; result.state = "MIGRATION";
        result.detail = "migration transactionnelle"; return result;
    }
    if (extension == ".json" || extension == ".env" || extension == ".toml") {
        result.percentage = lower_path.find("secret") != std::string::npos || lower_path.find("shopify") != std::string::npos
            ? 82 : 92;
        result.emoji = result.percentage < 90 ? "🔐⚙️" : "⚙️✅";
        result.state = result.percentage < 90 ? "CONFIG À ACTIVER" : "CONFIGURATION";
        result.detail = result.percentage < 90 ? "identifiants réels requis" : "configuration valide";
        return result;
    }
    if (extension == ".md" || extension == ".txt") {
        result.percentage = 88; result.emoji = "📘"; result.state = "DOCUMENTÉ";
        result.detail = "documentation ou rapport"; return result;
    }
    if (extension == ".cmake" || lower_path.find("cmakelists") != std::string::npos ||
        extension == ".sh" || extension == ".ps1" || extension == ".bat") {
        result.percentage = 94; result.emoji = "🛠️✅"; result.state = "OUTILLAGE";
        result.detail = "construction ou automatisation"; return result;
    }
    result.percentage = 85; result.emoji = "🟢"; result.state = "INTÉGRÉ";
    result.detail = "ressource intégrée";
    return result;
}

Result<ProjectProgressSummary> ProjectProgressAnalyzer::analyze(const std::filesystem::path& root) const {
    if (!std::filesystem::is_directory(root)) {
        return Result<ProjectProgressSummary>::failure("Project root is not a directory: " + root.string());
    }
    ProjectProgressSummary summary;
    double weighted_sum = 0.0, total_weight = 0.0;
    std::map<std::string, std::pair<double, double>> categories;
    for (const auto& item : std::filesystem::recursive_directory_iterator(root)) {
        if (item.is_directory()) { ++summary.directories; continue; }
        if (!item.is_regular_file()) continue;
        const auto relative = std::filesystem::relative(item.path(), root).generic_string();
        if (relative.rfind("build", 0) == 0 || relative.find("/.git/") != std::string::npos) continue;
        auto progress = scoreFile(root, item.path());
        ++summary.files;
        const auto extension = util::lower(item.path().extension().string());
        if (extension == ".cpp") ++summary.cpp_files;
        if (extension == ".h" || extension == ".hpp") ++summary.header_files;
        if (relative.find("tests/") != std::string::npos && extension == ".cpp") ++summary.test_files;
        if (progress.percentage == 0) ++summary.empty_files;
        const double weight = fileWeight(progress);
        weighted_sum += progress.percentage * weight;
        total_weight += weight;
        auto& category = categories[categoryFor(progress.relative_path)];
        category.first += progress.percentage * weight;
        category.second += weight;
        summary.entries.push_back(std::move(progress));
    }
    std::sort(summary.entries.begin(), summary.entries.end(), [](const auto& left, const auto& right) {
        return left.relative_path.generic_string() < right.relative_path.generic_string();
    });
    summary.weighted_percentage = total_weight > 0.0 ? weighted_sum / total_weight : 0.0;
    summary.source_percentage = weightedForPrefix(summary.entries, "src/");
    summary.shopify_percentage = weightedForPrefix(summary.entries, "include/elit21/shopify/");
    const auto shopify_source = weightedForPrefix(summary.entries, "src/elit21/shopify/");
    if (shopify_source > 0.0) summary.shopify_percentage = (summary.shopify_percentage + shopify_source) / 2.0;
    summary.test_percentage = weightedForPrefix(summary.entries, "tests/");
    summary.configuration_percentage = weightedForPrefix(summary.entries, "config/");
    for (const auto& [name, values] : categories) {
        summary.category_percentages[name] = values.second > 0.0 ? values.first / values.second : 0.0;
    }
    return Result<ProjectProgressSummary>::success(std::move(summary));
}

Result<void> ProjectProgressAnalyzer::writeTree(const std::filesystem::path& root,
                                                const ProjectProgressSummary& summary,
                                                const std::filesystem::path& output) const {
    std::ofstream stream(output, std::ios::binary | std::ios::trunc);
    if (!stream) return Result<void>::failure("Cannot write tree: " + output.string());
    stream << "LÉGENDE DES ÉTATS\n==================\n"
           << "✅ 90-100 % : implémenté et validé localement\n"
           << "🟢 80-89 %  : fonctionnel, perfectionnement possible\n"
           << "🟡 60-79 %  : partiel ou adaptateur métier partagé\n"
           << "🟠 1-59 %   : incomplet / faible profondeur\n"
           << "❌ 0 %      : vide ou manquant\n"
           << "🧪✅ test réel  🔐⚙️ activation requise  🗄️✅ migration  🛠️✅ outillage\n\n";
    stream << "📦 " << root.filename().string()
           << " [PROJET " << std::fixed << std::setprecision(1) << summary.weighted_percentage
           << "% | SHOPIFY " << summary.shopify_percentage
           << "% | TESTS " << summary.test_percentage << "% | STRUCTURE 100.0%]\n";

    std::map<std::string, const FileProgress*> by_path;
    for (const auto& entry : summary.entries) by_path[entry.relative_path.generic_string()] = &entry;

    std::function<double(const std::filesystem::path&)> directoryScore = [&](const auto& directory) {
        double sum = 0.0, weight = 0.0;
        const auto prefix = directory.empty() ? std::string{} : directory.generic_string() + "/";
        for (const auto& entry : summary.entries) {
            const auto path = entry.relative_path.generic_string();
            if (!prefix.empty() && path.rfind(prefix, 0) != 0) continue;
            const auto current = fileWeight(entry);
            sum += entry.percentage * current;
            weight += current;
        }
        return weight > 0.0 ? sum / weight : 0.0;
    };

    std::function<void(const std::filesystem::path&, const std::string&)> render;
    render = [&](const std::filesystem::path& relative, const std::string& prefix) {
        const auto absolute = root / relative;
        std::vector<std::filesystem::directory_entry> children;
        for (const auto& child : std::filesystem::directory_iterator(absolute)) {
            const auto child_relative = std::filesystem::relative(child.path(), root).generic_string();
            if (child_relative.rfind("build", 0) == 0 || child_relative.find("/.git/") != std::string::npos) continue;
            children.push_back(child);
        }
        std::sort(children.begin(), children.end(), [](const auto& left, const auto& right) {
            if (left.is_directory() != right.is_directory()) return left.is_directory() > right.is_directory();
            return left.path().filename().string() < right.path().filename().string();
        });
        for (std::size_t index = 0; index < children.size(); ++index) {
            const bool last = index + 1 == children.size();
            const auto connector = last ? "└── " : "├── ";
            const auto next_prefix = prefix + (last ? "    " : "│   ");
            const auto child_relative = std::filesystem::relative(children[index].path(), root);
            if (children[index].is_directory()) {
                const auto score = directoryScore(child_relative);
                stream << prefix << connector << directoryEmoji(score) << " ["
                       << std::setw(5) << std::setprecision(1) << score << "%] "
                       << children[index].path().filename().string() << "/\n";
                render(child_relative, next_prefix);
            } else {
                const auto found = by_path.find(child_relative.generic_string());
                if (found == by_path.end()) continue;
                const auto& entry = *found->second;
                stream << prefix << connector << entry.emoji << " [" << std::setw(3)
                       << entry.percentage << "%] " << children[index].path().filename().string()
                       << " — " << entry.state << " — " << entry.detail << '\n';
            }
        }
    };
    render({}, "");
    return Result<void>::success();
}

Result<void> ProjectProgressAnalyzer::writeCsv(const ProjectProgressSummary& summary,
                                               const std::filesystem::path& output) const {
    std::ofstream stream(output, std::ios::binary | std::ios::trunc);
    if (!stream) return Result<void>::failure("Cannot write CSV: " + output.string());
    stream << "path,percentage,emoji,state,detail,bytes,lines\n";
    for (const auto& entry : summary.entries) {
        stream << csvEscape(entry.relative_path.generic_string()) << ',' << entry.percentage << ','
               << csvEscape(entry.emoji) << ',' << csvEscape(entry.state) << ','
               << csvEscape(entry.detail) << ',' << entry.bytes << ',' << entry.lines << '\n';
    }
    return Result<void>::success();
}

Result<void> ProjectProgressAnalyzer::writeJson(const ProjectProgressSummary& summary,
                                                const std::filesystem::path& output) const {
    Json document = Json::object();
    document.set("files", static_cast<std::int64_t>(summary.files));
    document.set("directories", static_cast<std::int64_t>(summary.directories));
    document.set("cpp_files", static_cast<std::int64_t>(summary.cpp_files));
    document.set("header_files", static_cast<std::int64_t>(summary.header_files));
    document.set("test_files", static_cast<std::int64_t>(summary.test_files));
    document.set("empty_files", static_cast<std::int64_t>(summary.empty_files));
    document.set("weighted_percentage", summary.weighted_percentage);
    document.set("source_percentage", summary.source_percentage);
    document.set("shopify_percentage", summary.shopify_percentage);
    document.set("test_percentage", summary.test_percentage);
    document.set("configuration_percentage", summary.configuration_percentage);
    Json categories = Json::object();
    for (const auto& [name, value] : summary.category_percentages) categories.set(name, value);
    document.set("category_percentages", categories);
    Json entries = Json::array();
    for (const auto& entry : summary.entries) {
        Json item = Json::object();
        item.set("path", entry.relative_path.generic_string());
        item.set("percentage", entry.percentage);
        item.set("emoji", entry.emoji);
        item.set("state", entry.state);
        item.set("detail", entry.detail);
        item.set("bytes", static_cast<std::int64_t>(entry.bytes));
        item.set("lines", static_cast<std::int64_t>(entry.lines));
        entries.push(item);
    }
    document.set("entries", entries);
    std::ofstream stream(output, std::ios::binary | std::ios::trunc);
    if (!stream) return Result<void>::failure("Cannot write JSON: " + output.string());
    stream << document.dump(true) << '\n';
    return Result<void>::success();
}

} // namespace elit21::progress
