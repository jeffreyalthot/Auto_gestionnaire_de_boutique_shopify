#include "elit21/shopify/ShopifyApiVersionCatalog.h"

#include "elit21/shopify/ShopifyClient.h"
#include "elit21/shopify/queries/ApiVersionQueries.h"
#include "elit21/storage/Database.h"
#include "elit21/util/TimeUtil.h"

#include <algorithm>

namespace elit21::shopify {
namespace {

std::string stateKey(const std::string& shop_domain) {
    return "shopify.api_versions." + (shop_domain.empty() ? std::string("default") : shop_domain);
}

bool stableHandle(const std::string& value) {
    return value.size() == 7 && value[4] == '-' &&
           std::all_of(value.begin(), value.begin() + 4, [](unsigned char c) { return c >= '0' && c <= '9'; }) &&
           std::all_of(value.begin() + 5, value.end(), [](unsigned char c) { return c >= '0' && c <= '9'; });
}

Result<ShopifyApiVersionCatalogSnapshot> parseSnapshotJson(const Json& root) {
    if (!root.isObject()) {
        return Result<ShopifyApiVersionCatalogSnapshot>::failure("Shopify API version snapshot must be an object");
    }
    ShopifyApiVersionCatalogSnapshot snapshot;
    snapshot.shop_domain = root.getString("shop_domain");
    snapshot.captured_at = root.getString("captured_at");
    const auto versions = root.get("versions");
    if (!versions.isArray()) {
        return Result<ShopifyApiVersionCatalogSnapshot>::failure("Shopify API version snapshot versions must be an array");
    }
    for (std::size_t index = 0; index < versions.size(); ++index) {
        const auto item = versions.at(index);
        ShopifyPublicApiVersion version;
        version.handle = item.getString("handle");
        version.display_name = item.getString("display_name");
        version.supported = item.getBool("supported");
        if (!version.handle.empty()) snapshot.versions.push_back(std::move(version));
    }
    return Result<ShopifyApiVersionCatalogSnapshot>::success(std::move(snapshot));
}

} // namespace

Json ShopifyPublicApiVersion::toJson() const {
    Json output = Json::object();
    output.set("handle", handle);
    output.set("display_name", display_name);
    output.set("supported", supported);
    return output;
}

bool ShopifyApiVersionCatalogSnapshot::supports(const std::string& handle) const {
    return std::any_of(versions.begin(), versions.end(), [&](const auto& version) {
        return version.handle == handle && version.supported;
    });
}

std::optional<std::string> ShopifyApiVersionCatalogSnapshot::latestSupported() const {
    std::optional<std::string> latest;
    for (const auto& version : versions) {
        if (!version.supported || !stableHandle(version.handle)) continue;
        if (!latest || version.handle > *latest) latest = version.handle;
    }
    return latest;
}

std::optional<std::string> ShopifyApiVersionCatalogSnapshot::nextAfter(const std::string& handle) const {
    std::optional<std::string> next;
    for (const auto& version : versions) {
        if (!version.supported || !stableHandle(version.handle) || version.handle <= handle) continue;
        if (!next || version.handle < *next) next = version.handle;
    }
    return next;
}

Json ShopifyApiVersionCatalogSnapshot::toJson() const {
    Json output = Json::object();
    output.set("shop_domain", shop_domain);
    output.set("captured_at", captured_at);
    Json values = Json::array();
    for (const auto& version : versions) values.push(version.toJson());
    output.set("versions", values);
    if (const auto latest = latestSupported()) output.set("latest_supported", *latest);
    return output;
}

std::string ShopifyApiVersionCatalog::query() {
    return queries::ApiVersionQueries::publicVersions().source();
}

Result<ShopifyApiVersionCatalogSnapshot> ShopifyApiVersionCatalog::parse(
    const Json& graphql_root,
    const std::string& shop_domain) {
    const auto data = graphql_root.contains("data") ? graphql_root.get("data") : graphql_root;
    const auto versions = data.get("publicApiVersions");
    if (!versions.isArray() || versions.size() == 0) {
        return Result<ShopifyApiVersionCatalogSnapshot>::failure(
            "Shopify publicApiVersions response contains no versions");
    }
    ShopifyApiVersionCatalogSnapshot snapshot;
    snapshot.shop_domain = shop_domain;
    snapshot.captured_at = util::utcNowIso();
    for (std::size_t index = 0; index < versions.size(); ++index) {
        const auto item = versions.at(index);
        ShopifyPublicApiVersion version;
        version.handle = item.getString("handle");
        version.display_name = item.getString("displayName", version.handle);
        version.supported = item.getBool("supported");
        if (version.handle.empty()) {
            return Result<ShopifyApiVersionCatalogSnapshot>::failure(
                "Shopify publicApiVersions contains an entry without handle");
        }
        snapshot.versions.push_back(std::move(version));
    }
    std::sort(snapshot.versions.begin(), snapshot.versions.end(), [](const auto& left, const auto& right) {
        return left.handle < right.handle;
    });
    return Result<ShopifyApiVersionCatalogSnapshot>::success(std::move(snapshot));
}

Result<ShopifyApiVersionCatalogSnapshot> ShopifyApiVersionCatalog::discover(ShopifyClient& client) {
    auto response = client.graphql(query(), Json::object(), 4);
    if (!response) return Result<ShopifyApiVersionCatalogSnapshot>::failure(response.error());
    return parse(response.value(), client.config().shop);
}

Result<void> ShopifyApiVersionCatalog::persist(
    Database& database,
    const ShopifyApiVersionCatalogSnapshot& snapshot) {
    if (!database.isOpen()) return Result<void>::failure("Database is not open");
    if (snapshot.versions.empty()) return Result<void>::failure("Cannot persist an empty Shopify API version catalog");
    return database.setRuntimeState(stateKey(snapshot.shop_domain), snapshot.toJson().dump());
}

Result<std::optional<ShopifyApiVersionCatalogSnapshot>> ShopifyApiVersionCatalog::load(
    Database& database,
    const std::string& shop_domain) {
    auto stored = database.runtimeState(stateKey(shop_domain));
    if (!stored) return Result<std::optional<ShopifyApiVersionCatalogSnapshot>>::failure(stored.error());
    if (!stored.value()) {
        return Result<std::optional<ShopifyApiVersionCatalogSnapshot>>::success(std::nullopt);
    }
    auto parsed = Json::parse(*stored.value());
    if (!parsed) return Result<std::optional<ShopifyApiVersionCatalogSnapshot>>::failure(parsed.error());
    auto snapshot = parseSnapshotJson(parsed.value());
    if (!snapshot) return Result<std::optional<ShopifyApiVersionCatalogSnapshot>>::failure(snapshot.error());
    return Result<std::optional<ShopifyApiVersionCatalogSnapshot>>::success(
        std::optional<ShopifyApiVersionCatalogSnapshot>{std::move(snapshot.value())});
}

Result<void> ShopifyApiVersionCatalog::requireSupported(
    const ShopifyApiVersionCatalogSnapshot& snapshot,
    const std::string& configured_version) {
    if (configured_version.empty()) return Result<void>::failure("Configured Shopify API version is empty");
    if (!snapshot.supports(configured_version)) {
        const auto latest = snapshot.latestSupported();
        return Result<void>::failure(
            "Configured Shopify API version " + configured_version +
            " is not reported as supported" +
            (latest ? "; latest supported version is " + *latest : std::string{}));
    }
    return Result<void>::success();
}

} // namespace elit21::shopify
