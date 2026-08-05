#include "elit21/storage/Database.h"

#include "elit21/json/Json.h"
#include "elit21/security/Crypto.h"
#include "elit21/shopify/ShopifyOrderStateMachine.h"
#include "elit21/shopify/ShopifyTimestamp.h"
#include "elit21/util/TimeUtil.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>

namespace elit21 {
namespace {

std::string columnText(sqlite3_stmt* statement, int column) {
    const auto* value = sqlite3_column_text(statement, column);
    return value ? reinterpret_cast<const char*>(value) : std::string{};
}

} // namespace

Database::Database() = default;

Database::~Database() {
    std::lock_guard lock(mutex_);
    if (db_) sqlite3_close(db_);
}

Result<void> Database::open(const std::string& path) {
    std::lock_guard lock(mutex_);
    if (db_) return Result<void>::success();
    if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
        const auto error = db_ ? std::string(sqlite3_errmsg(db_)) : std::string("sqlite open failed");
        if (db_) sqlite3_close(db_);
        db_ = nullptr;
        return Result<void>::failure(error);
    }
    sqlite3_busy_timeout(db_, 5000);
    auto pragma = executeUnlocked(
        "PRAGMA foreign_keys=ON;"
        "PRAGMA journal_mode=WAL;"
        "PRAGMA synchronous=NORMAL;"
        "PRAGMA temp_store=MEMORY;");
    if (!pragma) return pragma;
    return Result<void>::success();
}

Result<void> Database::executeUnlocked(const std::string& sql) {
    if (!db_) return Result<void>::failure("Database is not open");
    char* error = nullptr;
    if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &error) != SQLITE_OK) {
        const std::string message = error ? error : "sqlite error";
        sqlite3_free(error);
        return Result<void>::failure(message);
    }
    return Result<void>::success();
}

Result<void> Database::execute(const std::string& sql) {
    std::lock_guard lock(mutex_);
    return executeUnlocked(sql);
}

Result<void> Database::migrate(const std::string& path) {
    std::ifstream file(path);
    if (!file) return Result<void>::failure("Migration introuvable: " + path);
    std::ostringstream sql;
    sql << file.rdbuf();
    return execute(sql.str());
}

std::string Database::sqlQuote(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size() + 2);
    escaped.push_back('\'');
    for (char character : value) {
        escaped.push_back(character);
        if (character == '\'') escaped.push_back('\'');
    }
    escaped.push_back('\'');
    return escaped;
}

Result<void> Database::migrateDirectory(const std::string& directory) {
    namespace fs = std::filesystem;
    std::vector<fs::path> migrations;
    const std::regex filename_pattern(R"(^\d{4}_.+\.sql$)");
    std::error_code error;
    if (!fs::exists(directory, error)) {
        return Result<void>::failure("Migration directory not found: " + directory);
    }
    for (const auto& entry : fs::directory_iterator(directory, error)) {
        if (error) return Result<void>::failure("Cannot enumerate migrations: " + error.message());
        if (entry.is_regular_file() && std::regex_match(entry.path().filename().string(), filename_pattern)) {
            migrations.push_back(entry.path());
        }
    }
    std::sort(migrations.begin(), migrations.end());

    std::lock_guard lock(mutex_);
    auto bootstrap = executeUnlocked(
        "CREATE TABLE IF NOT EXISTS schema_migrations("
        "version TEXT PRIMARY KEY,name TEXT NOT NULL,checksum TEXT NOT NULL DEFAULT '',"
        "applied_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP);");
    if (!bootstrap) return bootstrap;

    for (const auto& path : migrations) {
        const auto filename = path.filename().string();
        const auto version = filename.substr(0, 4);
        std::ifstream file(path);
        if (!file) return Result<void>::failure("Cannot read migration: " + path.string());
        std::ostringstream buffer;
        buffer << file.rdbuf();
        const auto sql = buffer.str();
        const auto checksum = crypto::sha256Hex(sql);

        sqlite3_stmt* check = nullptr;
        if (sqlite3_prepare_v2(db_, "SELECT checksum FROM schema_migrations WHERE version=?", -1, &check, nullptr) != SQLITE_OK) {
            return Result<void>::failure(sqlite3_errmsg(db_));
        }
        sqlite3_bind_text(check, 1, version.c_str(), -1, SQLITE_TRANSIENT);
        const bool already_applied = sqlite3_step(check) == SQLITE_ROW;
        const std::string stored_checksum = already_applied ? columnText(check, 0) : std::string{};
        sqlite3_finalize(check);
        if (already_applied) {
            if (!stored_checksum.empty() && stored_checksum != checksum) {
                return Result<void>::failure(
                    "Migration checksum mismatch for " + filename + ": applied=" + stored_checksum +
                    " current=" + checksum);
            }
            continue;
        }

        const std::string transaction =
            "BEGIN IMMEDIATE;\n" + sql + "\nINSERT INTO schema_migrations(version,name,checksum) VALUES(" +
            sqlQuote(version) + "," + sqlQuote(filename) + "," + sqlQuote(checksum) + ");\nCOMMIT;";
        auto applied = executeUnlocked(transaction);
        if (!applied) {
            executeUnlocked("ROLLBACK;");
            return Result<void>::failure("Migration " + filename + " failed: " + applied.error());
        }
    }
    return Result<void>::success();
}

Result<void> Database::prepareAndStep(const std::string& sql,
                                      const std::function<void(sqlite3_stmt*)>& binder) {
    std::lock_guard lock(mutex_);
    if (!db_) return Result<void>::failure("Database is not open");
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &statement, nullptr) != SQLITE_OK) {
        return Result<void>::failure(sqlite3_errmsg(db_));
    }
    binder(statement);
    const int code = sqlite3_step(statement);
    const std::string error = code == SQLITE_DONE ? std::string{} : std::string(sqlite3_errmsg(db_));
    sqlite3_finalize(statement);
    return code == SQLITE_DONE ? Result<void>::success() : Result<void>::failure(error);
}

Result<void> Database::upsertProduct(const StoredProduct& product) {
    return prepareAndStep(
        "INSERT INTO products(aliexpress_product_id,shopify_product_id,title,supplier_cost_cad,"
        "shipping_cost_cad,sale_price_cad,stock,status,updated_at) VALUES(?,?,?,?,?,?,?,?,?) "
        "ON CONFLICT(aliexpress_product_id) DO UPDATE SET shopify_product_id=excluded.shopify_product_id,"
        "title=excluded.title,supplier_cost_cad=excluded.supplier_cost_cad,"
        "shipping_cost_cad=excluded.shipping_cost_cad,sale_price_cad=excluded.sale_price_cad,"
        "stock=excluded.stock,status=excluded.status,updated_at=excluded.updated_at",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, product.ae_id.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, product.shopify_id.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 3, product.title.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(statement, 4, product.cost);
            sqlite3_bind_double(statement, 5, product.shipping);
            sqlite3_bind_double(statement, 6, product.price);
            sqlite3_bind_int(statement, 7, std::max(0, product.stock));
            sqlite3_bind_text(statement, 8, product.status.c_str(), -1, SQLITE_TRANSIENT);
            const auto now = util::utcNowIso();
            sqlite3_bind_text(statement, 9, now.c_str(), -1, SQLITE_TRANSIENT);
        });
}

Result<std::vector<StoredProduct>> Database::products(int limit) {
    std::lock_guard lock(mutex_);
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(
            db_,
            "SELECT id,aliexpress_product_id,COALESCE(shopify_product_id,''),title,"
            "supplier_cost_cad,shipping_cost_cad,sale_price_cad,stock,status "
            "FROM products ORDER BY id LIMIT ?",
            -1, &statement, nullptr) != SQLITE_OK) {
        return Result<std::vector<StoredProduct>>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_int(statement, 1, std::max(1, limit));
    std::vector<StoredProduct> output;
    while (sqlite3_step(statement) == SQLITE_ROW) {
        StoredProduct product;
        product.id = sqlite3_column_int64(statement, 0);
        product.ae_id = columnText(statement, 1);
        product.shopify_id = columnText(statement, 2);
        product.title = columnText(statement, 3);
        product.cost = sqlite3_column_double(statement, 4);
        product.shipping = sqlite3_column_double(statement, 5);
        product.price = sqlite3_column_double(statement, 6);
        product.stock = sqlite3_column_int(statement, 7);
        product.status = columnText(statement, 8);
        output.push_back(std::move(product));
    }
    sqlite3_finalize(statement);
    return Result<std::vector<StoredProduct>>::success(std::move(output));
}


Result<std::optional<StoredProduct>> Database::productByAliExpressId(
    const std::string& aliexpress_product_id) {
    if (aliexpress_product_id.empty()) {
        return Result<std::optional<StoredProduct>>::failure("AliExpress product ID is empty");
    }
    std::lock_guard lock(mutex_);
    if (!db_) return Result<std::optional<StoredProduct>>::failure("Database is not open");
    sqlite3_stmt* statement = nullptr;
    const char* sql =
        "SELECT id,aliexpress_product_id,COALESCE(shopify_product_id,''),title,"
        "supplier_cost_cad,shipping_cost_cad,sale_price_cad,stock,status "
        "FROM products WHERE aliexpress_product_id=? LIMIT 1";
    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        return Result<std::optional<StoredProduct>>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(statement, 1, aliexpress_product_id.c_str(), -1, SQLITE_TRANSIENT);
    std::optional<StoredProduct> output;
    if (sqlite3_step(statement) == SQLITE_ROW) {
        StoredProduct product;
        product.id = sqlite3_column_int64(statement, 0);
        product.ae_id = columnText(statement, 1);
        product.shopify_id = columnText(statement, 2);
        product.title = columnText(statement, 3);
        product.cost = sqlite3_column_double(statement, 4);
        product.shipping = sqlite3_column_double(statement, 5);
        product.price = sqlite3_column_double(statement, 6);
        product.stock = sqlite3_column_int(statement, 7);
        product.status = columnText(statement, 8);
        output = std::move(product);
    }
    sqlite3_finalize(statement);
    return Result<std::optional<StoredProduct>>::success(std::move(output));
}

Result<void> Database::upsertVariant(const StoredVariant& variant) {
    return prepareAndStep(
        "INSERT INTO variants(product_id,aliexpress_sku_id,shopify_variant_id,shopify_inventory_item_id,sku,"
        "supplier_cost_cad,shipping_cost_cad,sale_price_cad,stock,shopify_quantity,status,updated_at) "
        "SELECT id,?,?,?,?,?,?,?,?,?,?,? FROM products WHERE aliexpress_product_id=? "
        "ON CONFLICT(product_id,aliexpress_sku_id) DO UPDATE SET "
        "shopify_variant_id=excluded.shopify_variant_id,shopify_inventory_item_id=excluded.shopify_inventory_item_id,"
        "sku=excluded.sku,supplier_cost_cad=excluded.supplier_cost_cad,shipping_cost_cad=excluded.shipping_cost_cad,"
        "sale_price_cad=excluded.sale_price_cad,stock=excluded.stock,"
        "shopify_quantity=CASE WHEN excluded.shopify_quantity>=0 THEN excluded.shopify_quantity ELSE variants.shopify_quantity END,"
        "status=excluded.status,updated_at=excluded.updated_at",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, variant.ae_sku_id.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, variant.shopify_variant_id.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 3, variant.shopify_inventory_item_id.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 4, variant.sku.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(statement, 5, variant.cost);
            sqlite3_bind_double(statement, 6, variant.shipping);
            sqlite3_bind_double(statement, 7, variant.price);
            sqlite3_bind_int(statement, 8, std::max(0, variant.stock));
            sqlite3_bind_int(statement, 9, variant.shopify_quantity);
            sqlite3_bind_text(statement, 10, variant.status.c_str(), -1, SQLITE_TRANSIENT);
            const auto now = util::utcNowIso();
            sqlite3_bind_text(statement, 11, now.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 12, variant.ae_product_id.c_str(), -1, SQLITE_TRANSIENT);
        });
}

Result<void> Database::updateVariantShopifyQuantity(
    const std::string& shopify_inventory_item_id,
    int quantity) {
    if (shopify_inventory_item_id.empty()) {
        return Result<void>::failure("Shopify inventory item ID is required");
    }
    if (quantity < 0) {
        return Result<void>::failure("Shopify inventory quantity cannot be negative");
    }
    return prepareAndStep(
        "UPDATE variants SET shopify_quantity=?,shopify_quantity_updated_at=?,updated_at=? "
        "WHERE shopify_inventory_item_id=?",
        [&](sqlite3_stmt* statement) {
            const auto now = util::utcNowIso();
            sqlite3_bind_int(statement, 1, quantity);
            sqlite3_bind_text(statement, 2, now.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 3, now.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 4, shopify_inventory_item_id.c_str(), -1, SQLITE_TRANSIENT);
        });
}

Result<std::vector<StoredVariant>> Database::variants(int limit) {
    std::lock_guard lock(mutex_);
    sqlite3_stmt* statement = nullptr;
    const char* sql =
        "SELECT v.id,v.product_id,p.aliexpress_product_id,v.aliexpress_sku_id,"
        "COALESCE(v.shopify_variant_id,''),COALESCE(v.shopify_inventory_item_id,''),v.sku,"
        "v.supplier_cost_cad,v.shipping_cost_cad,v.sale_price_cad,v.stock,COALESCE(v.shopify_quantity,-1),v.status "
        "FROM variants v JOIN products p ON p.id=v.product_id ORDER BY v.id LIMIT ?";
    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        return Result<std::vector<StoredVariant>>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_int(statement, 1, std::max(1, limit));
    std::vector<StoredVariant> output;
    while (sqlite3_step(statement) == SQLITE_ROW) {
        StoredVariant value;
        value.id = sqlite3_column_int64(statement, 0);
        value.product_id = sqlite3_column_int64(statement, 1);
        value.ae_product_id = columnText(statement, 2);
        value.ae_sku_id = columnText(statement, 3);
        value.shopify_variant_id = columnText(statement, 4);
        value.shopify_inventory_item_id = columnText(statement, 5);
        value.sku = columnText(statement, 6);
        value.cost = sqlite3_column_double(statement, 7);
        value.shipping = sqlite3_column_double(statement, 8);
        value.price = sqlite3_column_double(statement, 9);
        value.stock = sqlite3_column_int(statement, 10);
        value.shopify_quantity = sqlite3_column_int(statement, 11);
        value.status = columnText(statement, 12);
        output.push_back(std::move(value));
    }
    sqlite3_finalize(statement);
    return Result<std::vector<StoredVariant>>::success(std::move(output));
}

Result<std::vector<StoredVariant>> Database::variantsForProduct(const std::string& aliexpress_product_id) {
    std::lock_guard lock(mutex_);
    sqlite3_stmt* statement = nullptr;
    const char* sql =
        "SELECT v.id,v.product_id,p.aliexpress_product_id,v.aliexpress_sku_id,"
        "COALESCE(v.shopify_variant_id,''),COALESCE(v.shopify_inventory_item_id,''),v.sku,"
        "v.supplier_cost_cad,v.shipping_cost_cad,v.sale_price_cad,v.stock,COALESCE(v.shopify_quantity,-1),v.status "
        "FROM variants v JOIN products p ON p.id=v.product_id WHERE p.aliexpress_product_id=? ORDER BY v.id";
    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        return Result<std::vector<StoredVariant>>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(statement, 1, aliexpress_product_id.c_str(), -1, SQLITE_TRANSIENT);
    std::vector<StoredVariant> output;
    while (sqlite3_step(statement) == SQLITE_ROW) {
        StoredVariant value;
        value.id = sqlite3_column_int64(statement, 0);
        value.product_id = sqlite3_column_int64(statement, 1);
        value.ae_product_id = columnText(statement, 2);
        value.ae_sku_id = columnText(statement, 3);
        value.shopify_variant_id = columnText(statement, 4);
        value.shopify_inventory_item_id = columnText(statement, 5);
        value.sku = columnText(statement, 6);
        value.cost = sqlite3_column_double(statement, 7);
        value.shipping = sqlite3_column_double(statement, 8);
        value.price = sqlite3_column_double(statement, 9);
        value.stock = sqlite3_column_int(statement, 10);
        value.shopify_quantity = sqlite3_column_int(statement, 11);
        value.status = columnText(statement, 12);
        output.push_back(std::move(value));
    }
    sqlite3_finalize(statement);
    return Result<std::vector<StoredVariant>>::success(std::move(output));
}


Result<std::optional<StoredVariant>> Database::variantByAliExpressSku(
    const std::string& aliexpress_product_id,
    const std::string& aliexpress_sku_id) {
    if (aliexpress_product_id.empty() || aliexpress_sku_id.empty()) {
        return Result<std::optional<StoredVariant>>::failure("AliExpress product and SKU IDs are required");
    }
    std::lock_guard lock(mutex_);
    if (!db_) return Result<std::optional<StoredVariant>>::failure("Database is not open");
    sqlite3_stmt* statement = nullptr;
    const char* sql =
        "SELECT v.id,v.product_id,p.aliexpress_product_id,v.aliexpress_sku_id,"
        "COALESCE(v.shopify_variant_id,''),COALESCE(v.shopify_inventory_item_id,''),v.sku,"
        "v.supplier_cost_cad,v.shipping_cost_cad,v.sale_price_cad,v.stock,COALESCE(v.shopify_quantity,-1),v.status "
        "FROM variants v JOIN products p ON p.id=v.product_id "
        "WHERE p.aliexpress_product_id=? AND v.aliexpress_sku_id=? LIMIT 1";
    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        return Result<std::optional<StoredVariant>>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(statement, 1, aliexpress_product_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, aliexpress_sku_id.c_str(), -1, SQLITE_TRANSIENT);
    std::optional<StoredVariant> output;
    if (sqlite3_step(statement) == SQLITE_ROW) {
        StoredVariant value;
        value.id = sqlite3_column_int64(statement, 0);
        value.product_id = sqlite3_column_int64(statement, 1);
        value.ae_product_id = columnText(statement, 2);
        value.ae_sku_id = columnText(statement, 3);
        value.shopify_variant_id = columnText(statement, 4);
        value.shopify_inventory_item_id = columnText(statement, 5);
        value.sku = columnText(statement, 6);
        value.cost = sqlite3_column_double(statement, 7);
        value.shipping = sqlite3_column_double(statement, 8);
        value.price = sqlite3_column_double(statement, 9);
        value.stock = sqlite3_column_int(statement, 10);
        value.shopify_quantity = sqlite3_column_int(statement, 11);
        value.status = columnText(statement, 12);
        output = std::move(value);
    }
    sqlite3_finalize(statement);
    return Result<std::optional<StoredVariant>>::success(std::move(output));
}

Result<bool> Database::saveOrder(const StoredOrder& order) {
    std::lock_guard lock(mutex_);
    sqlite3_stmt* statement = nullptr;
    const char* sql =
        "INSERT OR IGNORE INTO orders(shopify_order_id,aliexpress_order_id,customer_email,total_cad,currency,"
        "status,payload_json,idempotency_key,created_at,updated_at) VALUES(?,?,?,?,?,?,?,?,?,?)";
    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        return Result<bool>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(statement, 1, order.shopify_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, order.ae_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, order.customer_email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(statement, 4, order.total);
    sqlite3_bind_text(statement, 5, order.currency.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 6, order.status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 7, order.payload.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 8, order.idempotency_key.c_str(), -1, SQLITE_TRANSIENT);
    const auto now = util::utcNowIso();
    sqlite3_bind_text(statement, 9, now.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 10, now.c_str(), -1, SQLITE_TRANSIENT);
    const int code = sqlite3_step(statement);
    const bool inserted = sqlite3_changes(db_) > 0;
    const std::string error = code == SQLITE_DONE ? std::string{} : std::string(sqlite3_errmsg(db_));
    sqlite3_finalize(statement);
    return code == SQLITE_DONE ? Result<bool>::success(inserted) : Result<bool>::failure(error);
}


Result<bool> Database::saveOrderWithLines(const StoredOrder& order,
                                          const std::vector<StoredOrderLine>& lines) {
    if (order.shopify_id.empty() || order.status.empty() || order.idempotency_key.empty())
        return Result<bool>::failure("Shopify order identity, status and idempotency key are required");
    if (lines.empty()) return Result<bool>::failure("Shopify order must contain at least one line");
    for (const auto& line : lines) {
        if (line.quantity <= 0 || line.unit_price_cad < 0.0)
            return Result<bool>::failure("Shopify order line quantity and price are invalid");
    }
    std::lock_guard lock(mutex_);
    if (!db_) return Result<bool>::failure("Database is not open");
    auto begin = executeUnlocked("BEGIN IMMEDIATE;");
    if (!begin) return Result<bool>::failure(begin.error());
    sqlite3_stmt* order_statement = nullptr;
    const char* order_sql =
        "INSERT OR IGNORE INTO orders(shopify_order_id,aliexpress_order_id,customer_email,total_cad,currency,"
        "status,payload_json,idempotency_key,created_at,updated_at) VALUES(?,?,?,?,?,?,?,?,?,?)";
    if (sqlite3_prepare_v2(db_, order_sql, -1, &order_statement, nullptr) != SQLITE_OK) {
        executeUnlocked("ROLLBACK;"); return Result<bool>::failure(sqlite3_errmsg(db_));
    }
    const auto now = util::utcNowIso();
    sqlite3_bind_text(order_statement,1,order.shopify_id.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(order_statement,2,order.ae_id.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(order_statement,3,order.customer_email.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_double(order_statement,4,order.total);
    sqlite3_bind_text(order_statement,5,order.currency.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(order_statement,6,order.status.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(order_statement,7,order.payload.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(order_statement,8,order.idempotency_key.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(order_statement,9,now.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(order_statement,10,now.c_str(),-1,SQLITE_TRANSIENT);
    const int order_code=sqlite3_step(order_statement); const bool inserted=sqlite3_changes(db_)>0;
    const std::string order_error=order_code==SQLITE_DONE?std::string{}:std::string(sqlite3_errmsg(db_));
    sqlite3_finalize(order_statement);
    if(order_code!=SQLITE_DONE){executeUnlocked("ROLLBACK;");return Result<bool>::failure(order_error);}
    if(!inserted){auto c=executeUnlocked("COMMIT;");return c?Result<bool>::success(false):Result<bool>::failure(c.error());}
    const auto order_id=sqlite3_last_insert_rowid(db_);
    sqlite3_stmt* line_statement=nullptr;
    const char* line_sql="INSERT INTO order_lines(order_id,shopify_line_item_id,shopify_variant_id,aliexpress_product_id,aliexpress_sku_id,sku,title,quantity,unit_price_cad,status) VALUES(?,?,?,?,?,?,?,?,?,?)";
    if(sqlite3_prepare_v2(db_,line_sql,-1,&line_statement,nullptr)!=SQLITE_OK){executeUnlocked("ROLLBACK;");return Result<bool>::failure(sqlite3_errmsg(db_));}
    for(const auto& line:lines){sqlite3_reset(line_statement);sqlite3_clear_bindings(line_statement);sqlite3_bind_int64(line_statement,1,order_id);sqlite3_bind_text(line_statement,2,line.shopify_line_id.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(line_statement,3,line.shopify_variant_id.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(line_statement,4,line.aliexpress_product_id.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(line_statement,5,line.aliexpress_sku_id.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(line_statement,6,line.sku.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(line_statement,7,line.title.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_int(line_statement,8,line.quantity);sqlite3_bind_double(line_statement,9,line.unit_price_cad);sqlite3_bind_text(line_statement,10,line.status.c_str(),-1,SQLITE_TRANSIENT);if(sqlite3_step(line_statement)!=SQLITE_DONE){auto e=std::string(sqlite3_errmsg(db_));sqlite3_finalize(line_statement);executeUnlocked("ROLLBACK;");return Result<bool>::failure(e);}}
    sqlite3_finalize(line_statement);
    const std::string event="INSERT INTO shopify_order_events(order_id,event_type,from_status,to_status,context_json,created_at) VALUES("+std::to_string(order_id)+",'order_received','',"+sqlQuote(order.status)+","+sqlQuote(order.payload)+","+sqlQuote(now)+");";
    auto recorded=executeUnlocked(event);if(!recorded){executeUnlocked("ROLLBACK;");return Result<bool>::failure(recorded.error());}
    auto commit=executeUnlocked("COMMIT;");return commit?Result<bool>::success(true):Result<bool>::failure(commit.error());
}

Result<std::vector<StoredOrderLine>> Database::orderLines(const std::string& shopify_order_id) {
    std::lock_guard lock(mutex_); if(!db_)return Result<std::vector<StoredOrderLine>>::failure("Database is not open");
    sqlite3_stmt* statement=nullptr;const char* sql="SELECT l.id,l.order_id,COALESCE(l.shopify_line_item_id,''),COALESCE(l.shopify_variant_id,''),l.aliexpress_product_id,l.aliexpress_sku_id,COALESCE(l.sku,''),COALESCE(l.title,''),l.quantity,l.unit_price_cad,l.status FROM order_lines l JOIN orders o ON o.id=l.order_id WHERE o.shopify_order_id=? ORDER BY l.id";
    if(sqlite3_prepare_v2(db_,sql,-1,&statement,nullptr)!=SQLITE_OK)return Result<std::vector<StoredOrderLine>>::failure(sqlite3_errmsg(db_));
    sqlite3_bind_text(statement,1,shopify_order_id.c_str(),-1,SQLITE_TRANSIENT);std::vector<StoredOrderLine> output;
    while(sqlite3_step(statement)==SQLITE_ROW){StoredOrderLine line;line.id=sqlite3_column_int64(statement,0);line.order_id=sqlite3_column_int64(statement,1);line.shopify_line_id=columnText(statement,2);line.shopify_variant_id=columnText(statement,3);line.aliexpress_product_id=columnText(statement,4);line.aliexpress_sku_id=columnText(statement,5);line.sku=columnText(statement,6);line.title=columnText(statement,7);line.quantity=sqlite3_column_int(statement,8);line.unit_price_cad=sqlite3_column_double(statement,9);line.status=columnText(statement,10);output.push_back(std::move(line));}
    sqlite3_finalize(statement);return Result<std::vector<StoredOrderLine>>::success(std::move(output));
}

Result<std::vector<StoredOrder>> Database::ordersByStatus(const std::string& status, int limit) {
    std::lock_guard lock(mutex_);
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(
            db_,
            "SELECT id,shopify_order_id,COALESCE(aliexpress_order_id,''),status,payload_json,"
            "idempotency_key,total_cad,COALESCE(customer_email,''),currency FROM orders WHERE status=? ORDER BY id LIMIT ?",
            -1, &statement, nullptr) != SQLITE_OK) {
        return Result<std::vector<StoredOrder>>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(statement, 1, status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(statement, 2, std::max(1, limit));
    std::vector<StoredOrder> output;
    while (sqlite3_step(statement) == SQLITE_ROW) {
        StoredOrder order;
        order.id = sqlite3_column_int64(statement, 0);
        order.shopify_id = columnText(statement, 1);
        order.ae_id = columnText(statement, 2);
        order.status = columnText(statement, 3);
        order.payload = columnText(statement, 4);
        order.idempotency_key = columnText(statement, 5);
        order.total = sqlite3_column_double(statement, 6);
        order.customer_email = columnText(statement, 7);
        order.currency = columnText(statement, 8);
        output.push_back(std::move(order));
    }
    sqlite3_finalize(statement);
    return Result<std::vector<StoredOrder>>::success(std::move(output));
}


Result<std::vector<StoredOrder>> Database::ordersByShopifyIds(const std::vector<std::string>& ids){if(ids.empty())return Result<std::vector<StoredOrder>>::success({});std::lock_guard lock(mutex_);if(!db_)return Result<std::vector<StoredOrder>>::failure("Database is not open");std::string marks;for(std::size_t i=0;i<ids.size();++i){if(i)marks+=',';marks+='?';}const std::string sql="SELECT id,shopify_order_id,COALESCE(aliexpress_order_id,''),status,payload_json,idempotency_key,total_cad,COALESCE(customer_email,''),currency FROM orders WHERE shopify_order_id IN("+marks+") ORDER BY id";sqlite3_stmt*s=nullptr;if(sqlite3_prepare_v2(db_,sql.c_str(),-1,&s,nullptr)!=SQLITE_OK)return Result<std::vector<StoredOrder>>::failure(sqlite3_errmsg(db_));for(std::size_t i=0;i<ids.size();++i)sqlite3_bind_text(s,int(i+1),ids[i].c_str(),-1,SQLITE_TRANSIENT);std::vector<StoredOrder>o;while(sqlite3_step(s)==SQLITE_ROW){StoredOrder x;x.id=sqlite3_column_int64(s,0);x.shopify_id=columnText(s,1);x.ae_id=columnText(s,2);x.status=columnText(s,3);x.payload=columnText(s,4);x.idempotency_key=columnText(s,5);x.total=sqlite3_column_double(s,6);x.customer_email=columnText(s,7);x.currency=columnText(s,8);o.push_back(std::move(x));}sqlite3_finalize(s);return Result<std::vector<StoredOrder>>::success(std::move(o));}
Result<int> Database::redactShopifyOrders(const std::vector<std::string>& ids,const std::string& reason){if(ids.empty())return Result<int>::success(0);std::lock_guard lock(mutex_);if(!db_)return Result<int>::failure("Database is not open");auto b=executeUnlocked("BEGIN IMMEDIATE;");if(!b)return Result<int>::failure(b.error());std::string marks;for(std::size_t i=0;i<ids.size();++i){if(i)marks+=',';marks+='?';}const std::string sql="UPDATE orders SET customer_email=NULL,payload_json=?,last_error=?,updated_at=? WHERE shopify_order_id IN("+marks+")";sqlite3_stmt*s=nullptr;if(sqlite3_prepare_v2(db_,sql.c_str(),-1,&s,nullptr)!=SQLITE_OK){executeUnlocked("ROLLBACK;");return Result<int>::failure(sqlite3_errmsg(db_));}Json j=Json::object();j.set("redacted",true);j.set("reason",reason);j.set("redacted_at",util::utcNowIso());auto now=util::utcNowIso(),body=j.dump();sqlite3_bind_text(s,1,body.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,2,reason.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,3,now.c_str(),-1,SQLITE_TRANSIENT);for(std::size_t i=0;i<ids.size();++i)sqlite3_bind_text(s,int(i+4),ids[i].c_str(),-1,SQLITE_TRANSIENT);int code=sqlite3_step(s),changed=sqlite3_changes(db_);auto err=code==SQLITE_DONE?std::string{}:std::string(sqlite3_errmsg(db_));sqlite3_finalize(s);if(code!=SQLITE_DONE){executeUnlocked("ROLLBACK;");return Result<int>::failure(err);}auto c=executeUnlocked("COMMIT;");return c?Result<int>::success(changed):Result<int>::failure(c.error());}
Result<int> Database::redactAllShopifyCustomerData(const std::string& reason){std::lock_guard lock(mutex_);if(!db_)return Result<int>::failure("Database is not open");auto b=executeUnlocked("BEGIN IMMEDIATE;");if(!b)return Result<int>::failure(b.error());Json j=Json::object();j.set("redacted",true);j.set("reason",reason);j.set("redacted_at",util::utcNowIso());auto r=executeUnlocked("UPDATE orders SET customer_email=NULL,payload_json="+sqlQuote(j.dump())+",last_error="+sqlQuote(reason)+",updated_at="+sqlQuote(util::utcNowIso())+";");if(!r){executeUnlocked("ROLLBACK;");return Result<int>::failure(r.error());}int changed=sqlite3_changes(db_);auto m=executeUnlocked("DELETE FROM customer_messages;");if(!m){executeUnlocked("ROLLBACK;");return Result<int>::failure(m.error());}auto c=executeUnlocked("COMMIT;");return c?Result<int>::success(changed):Result<int>::failure(c.error());}
Result<bool> Database::transitionOrderStatus(const std::string&id,const std::vector<std::string>&expected,const std::string&next,const std::string&event,const std::string&context,const std::string&last_error){if(id.empty()||expected.empty()||next.empty()||event.empty())return Result<bool>::failure("Order transition parameters are incomplete");for(const auto&s:expected){auto v=shopify::ShopifyOrderStateMachine::validateTransition(s,next);if(!v)return Result<bool>::failure(v.error());}std::lock_guard lock(mutex_);if(!db_)return Result<bool>::failure("Database is not open");auto b=executeUnlocked("BEGIN IMMEDIATE;");if(!b)return Result<bool>::failure(b.error());std::string states;for(std::size_t i=0;i<expected.size();++i){if(i)states+=',';states+=sqlQuote(expected[i]);}auto now=util::utcNowIso();auto u=executeUnlocked("UPDATE orders SET status="+sqlQuote(next)+",last_error="+sqlQuote(last_error)+",updated_at="+sqlQuote(now)+" WHERE shopify_order_id="+sqlQuote(id)+" AND status IN("+states+");");if(!u){executeUnlocked("ROLLBACK;");return Result<bool>::failure(u.error());}if(sqlite3_changes(db_)==0){auto c=executeUnlocked("COMMIT;");return c?Result<bool>::success(false):Result<bool>::failure(c.error());}auto e=executeUnlocked("INSERT INTO shopify_order_events(order_id,event_type,from_status,to_status,context_json,created_at) SELECT id,"+sqlQuote(event)+","+sqlQuote(expected.front())+","+sqlQuote(next)+","+sqlQuote(context)+","+sqlQuote(now)+" FROM orders WHERE shopify_order_id="+sqlQuote(id)+";");if(!e){executeUnlocked("ROLLBACK;");return Result<bool>::failure(e.error());}auto c=executeUnlocked("COMMIT;");return c?Result<bool>::success(true):Result<bool>::failure(c.error());}

Result<bool> Database::setSupplierOrderAndTransition(const std::string& shopify_id,
                                                            const std::string& expected_status,
                                                            const std::string& aliexpress_id,
                                                            const std::string& next_status,
                                                            const std::string& context_json) {
    if (shopify_id.empty() || expected_status.empty() || aliexpress_id.empty() || next_status.empty())
        return Result<bool>::failure("Supplier-order transition parameters are incomplete");
    auto valid = shopify::ShopifyOrderStateMachine::validateTransition(expected_status, next_status);
    if (!valid) return Result<bool>::failure(valid.error());
    std::lock_guard lock(mutex_);
    if (!db_) return Result<bool>::failure("Database is not open");
    auto begin = executeUnlocked("BEGIN IMMEDIATE;");
    if (!begin) return Result<bool>::failure(begin.error());
    const auto now = util::utcNowIso();
    sqlite3_stmt* statement = nullptr;
    const char* sql = "UPDATE orders SET aliexpress_order_id=?,status=?,last_error='',updated_at=? "
                      "WHERE shopify_order_id=? AND status=?";
    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        executeUnlocked("ROLLBACK;"); return Result<bool>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(statement, 1, aliexpress_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, next_status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, now.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, shopify_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 5, expected_status.c_str(), -1, SQLITE_TRANSIENT);
    const int code = sqlite3_step(statement);
    const bool changed = sqlite3_changes(db_) > 0;
    const auto error = code == SQLITE_DONE ? std::string{} : std::string(sqlite3_errmsg(db_));
    sqlite3_finalize(statement);
    if (code != SQLITE_DONE) { executeUnlocked("ROLLBACK;"); return Result<bool>::failure(error); }
    if (!changed) {
        auto commit = executeUnlocked("COMMIT;");
        return commit ? Result<bool>::success(false) : Result<bool>::failure(commit.error());
    }
    const auto event = executeUnlocked(
        "INSERT INTO shopify_order_events(order_id,event_type,from_status,to_status,context_json,created_at) "
        "SELECT id,'supplier_order_created'," + sqlQuote(expected_status) + ',' + sqlQuote(next_status) + ',' +
        sqlQuote(context_json) + ',' + sqlQuote(now) + " FROM orders WHERE shopify_order_id=" + sqlQuote(shopify_id));
    if (!event) { executeUnlocked("ROLLBACK;"); return Result<bool>::failure(event.error()); }
    auto commit = executeUnlocked("COMMIT;");
    return commit ? Result<bool>::success(true) : Result<bool>::failure(commit.error());
}

Result<void> Database::updateOrderSupplierId(const std::string& shopify_id,
                                             const std::string& aliexpress_id,
                                             const std::string& status) {
    return prepareAndStep(
        "UPDATE orders SET aliexpress_order_id=?,status=?,updated_at=? WHERE shopify_order_id=?",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, aliexpress_id.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, status.c_str(), -1, SQLITE_TRANSIENT);
            const auto now = util::utcNowIso();
            sqlite3_bind_text(statement, 3, now.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 4, shopify_id.c_str(), -1, SQLITE_TRANSIENT);
        });
}

Result<void> Database::updateOrderStatus(const std::string& shopify_id,
                                         const std::string& status,
                                         const std::string& last_error) {
    if (shopify_id.empty() || status.empty()) {
        return Result<void>::failure("Shopify order id and status are required");
    }
    return prepareAndStep(
        "UPDATE orders SET status=?,last_error=?,updated_at=? WHERE shopify_order_id=?",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, status.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, last_error.c_str(), -1, SQLITE_TRANSIENT);
            const auto now = util::utcNowIso();
            sqlite3_bind_text(statement, 3, now.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 4, shopify_id.c_str(), -1, SQLITE_TRANSIENT);
        });
}

Result<void> Database::saveShipment(long long order_id, const ShipmentInfo& shipment) {
    return prepareAndStep(
        "INSERT INTO shipments(order_id,tracking_number,carrier,status,last_event,updated_at) "
        "VALUES(?,?,?,?,?,?)",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_int64(statement, 1, order_id);
            sqlite3_bind_text(statement, 2, shipment.tracking_number.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 3, shipment.carrier.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 4, shipment.status.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 5, shipment.last_event.c_str(), -1, SQLITE_TRANSIENT);
            const auto now = util::utcNowIso();
            sqlite3_bind_text(statement, 6, now.c_str(), -1, SQLITE_TRANSIENT);
        });
}

Result<long long> Database::enqueueTask(const std::string& kind,
                                        const std::string& payload_json,
                                        int priority,
                                        const std::string& run_after) {
    std::lock_guard lock(mutex_);
    if (!db_) return Result<long long>::failure("Database is not open");
    sqlite3_stmt* statement = nullptr;
    const char* sql =
        "INSERT INTO tasks(kind,payload_json,status,priority,run_after,created_at,updated_at) "
        "VALUES(?,?,'pending',?,?,?,?)";
    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        return Result<long long>::failure(sqlite3_errmsg(db_));
    }
    const auto now = util::utcNowIso();
    const auto due = run_after.empty() ? now : run_after;
    sqlite3_bind_text(statement, 1, kind.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, payload_json.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(statement, 3, priority);
    sqlite3_bind_text(statement, 4, due.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 5, now.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 6, now.c_str(), -1, SQLITE_TRANSIENT);
    const int code = sqlite3_step(statement);
    const auto id = sqlite3_last_insert_rowid(db_);
    const std::string error = code == SQLITE_DONE ? std::string{} : std::string(sqlite3_errmsg(db_));
    sqlite3_finalize(statement);
    return code == SQLITE_DONE ? Result<long long>::success(id) : Result<long long>::failure(error);
}

Result<TaskEnqueueResult> Database::enqueueUniqueTask(const std::string& kind,
                                                       const std::string& payload_json,
                                                       int priority,
                                                       const std::string& run_after,
                                                       const std::string& dedupe_key) {
    if (dedupe_key.empty()) {
        auto queued = enqueueTask(kind, payload_json, priority, run_after);
        if (!queued) return Result<TaskEnqueueResult>::failure(queued.error());
        return Result<TaskEnqueueResult>::success({queued.value(), true});
    }

    std::lock_guard lock(mutex_);
    if (!db_) return Result<TaskEnqueueResult>::failure("Database is not open");
    auto begin = executeUnlocked("BEGIN IMMEDIATE;");
    if (!begin) return Result<TaskEnqueueResult>::failure(begin.error());

    sqlite3_stmt* existing = nullptr;
    if (sqlite3_prepare_v2(
            db_,
            "SELECT id FROM tasks WHERE dedupe_key=? AND status IN ('pending','processing') ORDER BY id DESC LIMIT 1",
            -1, &existing, nullptr) != SQLITE_OK) {
        executeUnlocked("ROLLBACK;");
        return Result<TaskEnqueueResult>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(existing, 1, dedupe_key.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(existing) == SQLITE_ROW) {
        const auto id = sqlite3_column_int64(existing, 0);
        sqlite3_finalize(existing);
        executeUnlocked("COMMIT;");
        return Result<TaskEnqueueResult>::success({id, false});
    }
    sqlite3_finalize(existing);

    sqlite3_stmt* insert = nullptr;
    const char* sql =
        "INSERT INTO tasks(kind,payload_json,status,priority,run_after,dedupe_key,created_at,updated_at) "
        "VALUES(?,?,'pending',?,?,?,?,?)";
    if (sqlite3_prepare_v2(db_, sql, -1, &insert, nullptr) != SQLITE_OK) {
        executeUnlocked("ROLLBACK;");
        return Result<TaskEnqueueResult>::failure(sqlite3_errmsg(db_));
    }
    const auto now = util::utcNowIso();
    const auto due = run_after.empty() ? now : run_after;
    sqlite3_bind_text(insert, 1, kind.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insert, 2, payload_json.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(insert, 3, priority);
    sqlite3_bind_text(insert, 4, due.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insert, 5, dedupe_key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insert, 6, now.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insert, 7, now.c_str(), -1, SQLITE_TRANSIENT);
    const int code = sqlite3_step(insert);
    const auto id = sqlite3_last_insert_rowid(db_);
    const std::string error = code == SQLITE_DONE ? std::string{} : std::string(sqlite3_errmsg(db_));
    sqlite3_finalize(insert);
    if (code != SQLITE_DONE) {
        executeUnlocked("ROLLBACK;");
        return Result<TaskEnqueueResult>::failure(error);
    }
    auto commit = executeUnlocked("COMMIT;");
    if (!commit) return Result<TaskEnqueueResult>::failure(commit.error());
    return Result<TaskEnqueueResult>::success({id, true});
}

Result<int> Database::recoverStaleTasks(int stale_after_seconds) {
    std::lock_guard lock(mutex_);
    if (!db_) return Result<int>::failure("Database is not open");
    sqlite3_stmt* statement = nullptr;
    const char* sql =
        "UPDATE tasks SET status='pending',locked_by=NULL,locked_at=NULL,"
        "last_error=CASE WHEN COALESCE(last_error,'')='' THEN 'Recovered stale worker lease' ELSE last_error END,"
        "updated_at=? WHERE status='processing' AND locked_at IS NOT NULL "
        "AND datetime(locked_at)<=datetime('now',?)";
    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        return Result<int>::failure(sqlite3_errmsg(db_));
    }
    const auto now = util::utcNowIso();
    const auto interval = "-" + std::to_string(std::max(1, stale_after_seconds)) + " seconds";
    sqlite3_bind_text(statement, 1, now.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, interval.c_str(), -1, SQLITE_TRANSIENT);
    const int code = sqlite3_step(statement);
    const int changed = sqlite3_changes(db_);
    const std::string error = code == SQLITE_DONE ? std::string{} : std::string(sqlite3_errmsg(db_));
    sqlite3_finalize(statement);
    return code == SQLITE_DONE ? Result<int>::success(changed) : Result<int>::failure(error);
}

Result<std::vector<TaskRecord>> Database::claimTasks(const std::string& worker_id, int limit) {
    std::lock_guard lock(mutex_);
    auto begin = executeUnlocked("BEGIN IMMEDIATE;");
    if (!begin) return Result<std::vector<TaskRecord>>::failure(begin.error());
    sqlite3_stmt* statement = nullptr;
    const char* sql =
        "SELECT id,kind,payload_json,status,priority,attempts,max_attempts,run_after,COALESCE(last_error,'') "
        "FROM tasks WHERE status='pending' AND run_after<=? AND attempts<max_attempts "
        "ORDER BY priority ASC,id ASC LIMIT ?";
    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        executeUnlocked("ROLLBACK;");
        return Result<std::vector<TaskRecord>>::failure(sqlite3_errmsg(db_));
    }
    const auto now = util::utcNowIso();
    sqlite3_bind_text(statement, 1, now.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(statement, 2, std::max(1, limit));
    std::vector<TaskRecord> tasks;
    while (sqlite3_step(statement) == SQLITE_ROW) {
        TaskRecord task;
        task.id = sqlite3_column_int64(statement, 0);
        task.kind = columnText(statement, 1);
        task.payload_json = columnText(statement, 2);
        task.status = columnText(statement, 3);
        task.priority = sqlite3_column_int(statement, 4);
        task.attempts = sqlite3_column_int(statement, 5);
        task.max_attempts = sqlite3_column_int(statement, 6);
        task.run_after = columnText(statement, 7);
        task.last_error = columnText(statement, 8);
        tasks.push_back(task);
    }
    sqlite3_finalize(statement);
    for (const auto& task : tasks) {
        const std::string update =
            "UPDATE tasks SET status='processing',attempts=attempts+1,locked_by=" + sqlQuote(worker_id) +
            ",locked_at=" + sqlQuote(now) + ",updated_at=" + sqlQuote(now) + " WHERE id=" +
            std::to_string(task.id) + " AND status='pending';";
        auto changed = executeUnlocked(update);
        if (!changed) {
            executeUnlocked("ROLLBACK;");
            return Result<std::vector<TaskRecord>>::failure(changed.error());
        }
    }
    auto commit = executeUnlocked("COMMIT;");
    if (!commit) return Result<std::vector<TaskRecord>>::failure(commit.error());
    return Result<std::vector<TaskRecord>>::success(std::move(tasks));
}

Result<void> Database::completeTask(long long task_id) {
    return prepareAndStep(
        "UPDATE tasks SET status='completed',locked_by=NULL,locked_at=NULL,updated_at=? WHERE id=?",
        [&](sqlite3_stmt* statement) {
            const auto now = util::utcNowIso();
            sqlite3_bind_text(statement, 1, now.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int64(statement, 2, task_id);
        });
}

Result<void> Database::failTask(long long task_id, const std::string& error, int retry_delay_seconds) {
    std::lock_guard lock(mutex_);
    if (!db_) return Result<void>::failure("Database is not open");
    auto begin = executeUnlocked("BEGIN IMMEDIATE;");
    if (!begin) return begin;

    sqlite3_stmt* update = nullptr;
    const char* update_sql =
        "UPDATE tasks SET status=CASE WHEN attempts>=max_attempts THEN 'dead' ELSE 'pending' END,"
        "last_error=?,run_after=datetime('now',?),locked_by=NULL,locked_at=NULL,updated_at=? WHERE id=?";
    if (sqlite3_prepare_v2(db_, update_sql, -1, &update, nullptr) != SQLITE_OK) {
        executeUnlocked("ROLLBACK;");
        return Result<void>::failure(sqlite3_errmsg(db_));
    }
    const auto delay = "+" + std::to_string(std::max(1, retry_delay_seconds)) + " seconds";
    const auto now = util::utcNowIso();
    sqlite3_bind_text(update, 1, error.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(update, 2, delay.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(update, 3, now.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(update, 4, task_id);
    const int update_code = sqlite3_step(update);
    const std::string update_error = update_code == SQLITE_DONE ? std::string{} : std::string(sqlite3_errmsg(db_));
    sqlite3_finalize(update);
    if (update_code != SQLITE_DONE) {
        executeUnlocked("ROLLBACK;");
        return Result<void>::failure(update_error);
    }

    sqlite3_stmt* dead_letter = nullptr;
    const char* dead_sql =
        "INSERT INTO dead_letters(source_table,source_id,category,payload_json,error_code,error_message) "
        "SELECT 'tasks',id,kind,payload_json,'TASK_RETRY_EXHAUSTED',? FROM tasks "
        "WHERE id=? AND status='dead' AND NOT EXISTS("
        "SELECT 1 FROM dead_letters WHERE source_table='tasks' AND source_id=? AND resolved_at IS NULL)";
    if (sqlite3_prepare_v2(db_, dead_sql, -1, &dead_letter, nullptr) != SQLITE_OK) {
        executeUnlocked("ROLLBACK;");
        return Result<void>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(dead_letter, 1, error.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(dead_letter, 2, task_id);
    sqlite3_bind_int64(dead_letter, 3, task_id);
    const int dead_code = sqlite3_step(dead_letter);
    const std::string dead_error = dead_code == SQLITE_DONE ? std::string{} : std::string(sqlite3_errmsg(db_));
    sqlite3_finalize(dead_letter);
    if (dead_code != SQLITE_DONE) {
        executeUnlocked("ROLLBACK;");
        return Result<void>::failure(dead_error);
    }
    return executeUnlocked("COMMIT;");
}

Result<bool> Database::recordWebhook(const std::string& provider,
                                     const std::string& webhook_id,
                                     const std::string& topic,
                                     const std::string& payload_hash) {
    if (provider.empty() || webhook_id.empty() || topic.empty() || payload_hash.empty()) {
        return Result<bool>::failure("Webhook identity, topic and payload hash are required");
    }
    std::lock_guard lock(mutex_);
    if (!db_) return Result<bool>::failure("Database is not open");
    auto begin = executeUnlocked("BEGIN IMMEDIATE;");
    if (!begin) return Result<bool>::failure(begin.error());

    sqlite3_stmt* insert = nullptr;
    if (sqlite3_prepare_v2(
            db_,
            "INSERT OR IGNORE INTO webhooks(provider,webhook_id,topic,payload_hash) VALUES(?,?,?,?)",
            -1, &insert, nullptr) != SQLITE_OK) {
        executeUnlocked("ROLLBACK;");
        return Result<bool>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(insert, 1, provider.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insert, 2, webhook_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insert, 3, topic.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insert, 4, payload_hash.c_str(), -1, SQLITE_TRANSIENT);
    const int code = sqlite3_step(insert);
    const bool inserted = sqlite3_changes(db_) > 0;
    const std::string insert_error = code == SQLITE_DONE ? std::string{} : std::string(sqlite3_errmsg(db_));
    sqlite3_finalize(insert);
    if (code != SQLITE_DONE) {
        executeUnlocked("ROLLBACK;");
        return Result<bool>::failure(insert_error);
    }
    if (inserted) {
        auto commit = executeUnlocked("COMMIT;");
        return commit ? Result<bool>::success(true) : Result<bool>::failure(commit.error());
    }

    sqlite3_stmt* query = nullptr;
    if (sqlite3_prepare_v2(
            db_,
            "SELECT payload_hash,status FROM webhooks WHERE provider=? AND webhook_id=?",
            -1, &query, nullptr) != SQLITE_OK) {
        executeUnlocked("ROLLBACK;");
        return Result<bool>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(query, 1, provider.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(query, 2, webhook_id.c_str(), -1, SQLITE_TRANSIENT);
    std::string existing_hash;
    std::string status;
    if (sqlite3_step(query) == SQLITE_ROW) {
        existing_hash = columnText(query, 0);
        status = columnText(query, 1);
    }
    sqlite3_finalize(query);
    if (existing_hash.empty()) {
        executeUnlocked("ROLLBACK;");
        return Result<bool>::failure("Existing webhook row could not be loaded");
    }
    if (existing_hash != payload_hash) {
        executeUnlocked("ROLLBACK;");
        return Result<bool>::failure("Webhook replay payload hash mismatch");
    }
    if (status == "failed") {
        sqlite3_stmt* retry = nullptr;
        if (sqlite3_prepare_v2(
                db_,
                "UPDATE webhooks SET status='received',topic=?,received_at=?,processed_at=NULL,last_error=NULL "
                "WHERE provider=? AND webhook_id=?",
                -1, &retry, nullptr) != SQLITE_OK) {
            executeUnlocked("ROLLBACK;");
            return Result<bool>::failure(sqlite3_errmsg(db_));
        }
        const auto now = util::utcNowIso();
        sqlite3_bind_text(retry, 1, topic.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(retry, 2, now.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(retry, 3, provider.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(retry, 4, webhook_id.c_str(), -1, SQLITE_TRANSIENT);
        const int retry_code = sqlite3_step(retry);
        const std::string retry_error = retry_code == SQLITE_DONE ? std::string{} : std::string(sqlite3_errmsg(db_));
        sqlite3_finalize(retry);
        if (retry_code != SQLITE_DONE) {
            executeUnlocked("ROLLBACK;");
            return Result<bool>::failure(retry_error);
        }
        auto commit = executeUnlocked("COMMIT;");
        return commit ? Result<bool>::success(true) : Result<bool>::failure(commit.error());
    }
    auto commit = executeUnlocked("COMMIT;");
    return commit ? Result<bool>::success(false) : Result<bool>::failure(commit.error());
}

Result<void> Database::setWebhookStatus(const std::string& provider,
                                        const std::string& webhook_id,
                                        const std::string& status,
                                        const std::string& error,
                                        bool terminal) {
    return prepareAndStep(
        terminal
            ? "UPDATE webhooks SET status=?,processed_at=?,last_error=? WHERE provider=? AND webhook_id=?"
            : "UPDATE webhooks SET status=?,processed_at=NULL,last_error=? WHERE provider=? AND webhook_id=?",
        [&](sqlite3_stmt* statement) {
            const auto now = util::utcNowIso();
            sqlite3_bind_text(statement, 1, status.c_str(), -1, SQLITE_TRANSIENT);
            int offset = 2;
            if (terminal) sqlite3_bind_text(statement, offset++, now.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, offset++, error.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, offset++, provider.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, offset, webhook_id.c_str(), -1, SQLITE_TRANSIENT);
        });
}

Result<void> Database::markWebhookQueued(const std::string& provider,
                                         const std::string& webhook_id) {
    return setWebhookStatus(provider, webhook_id, "queued", {}, false);
}

Result<void> Database::markWebhookProcessing(const std::string& provider,
                                             const std::string& webhook_id) {
    return setWebhookStatus(provider, webhook_id, "processing", {}, false);
}

Result<void> Database::markWebhookRetrying(const std::string& provider,
                                           const std::string& webhook_id,
                                           const std::string& error) {
    return setWebhookStatus(provider, webhook_id, "queued", error, false);
}

Result<void> Database::markWebhookProcessed(const std::string& provider,
                                            const std::string& webhook_id,
                                            const std::string& error) {
    return setWebhookStatus(provider, webhook_id, error.empty() ? "processed" : "failed", error, true);
}


Result<bool> Database::acquireLease(const std::string&name,const std::string&owner,int ttl){if(name.empty()||owner.empty())return Result<bool>::failure("Lease name and owner are required");std::lock_guard lock(mutex_);if(!db_)return Result<bool>::failure("Database is not open");sqlite3_stmt*s=nullptr;const char*sql="INSERT INTO shopify_sync_leases(lease_name,owner_id,acquired_at,expires_at) VALUES(?,?,CURRENT_TIMESTAMP,datetime('now',?)) ON CONFLICT(lease_name) DO UPDATE SET owner_id=excluded.owner_id,acquired_at=CURRENT_TIMESTAMP,expires_at=excluded.expires_at WHERE shopify_sync_leases.owner_id=excluded.owner_id OR datetime(shopify_sync_leases.expires_at)<=datetime('now')";if(sqlite3_prepare_v2(db_,sql,-1,&s,nullptr)!=SQLITE_OK)return Result<bool>::failure(sqlite3_errmsg(db_));auto interval="+"+std::to_string(std::max(1,ttl))+" seconds";sqlite3_bind_text(s,1,name.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,2,owner.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,3,interval.c_str(),-1,SQLITE_TRANSIENT);int code=sqlite3_step(s);bool acquired=sqlite3_changes(db_)>0;auto err=code==SQLITE_DONE?std::string{}:std::string(sqlite3_errmsg(db_));sqlite3_finalize(s);return code==SQLITE_DONE?Result<bool>::success(acquired):Result<bool>::failure(err);}
Result<void> Database::releaseLease(const std::string&name,const std::string&owner){return prepareAndStep("DELETE FROM shopify_sync_leases WHERE lease_name=? AND owner_id=?",[&](sqlite3_stmt*s){sqlite3_bind_text(s,1,name.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,2,owner.c_str(),-1,SQLITE_TRANSIENT);});}
Result<void> Database::recordShopifyPrivacyRequest(const ShopifyPrivacyRequestRecord&r){if(r.request_id.empty()||r.topic.empty()||r.shop_domain.empty()||r.payload_hash.empty())return Result<void>::failure("Shopify privacy request identity is incomplete");std::lock_guard lock(mutex_);if(!db_)return Result<void>::failure("Database is not open");auto b=executeUnlocked("BEGIN IMMEDIATE;");if(!b)return b;sqlite3_stmt*s=nullptr;const char*sql="INSERT OR IGNORE INTO shopify_privacy_requests(request_id,topic,shop_domain,customer_id,order_ids_json,payload_hash,status,received_at,updated_at) VALUES(?,?,?,?,?,?,?,CURRENT_TIMESTAMP,CURRENT_TIMESTAMP)";if(sqlite3_prepare_v2(db_,sql,-1,&s,nullptr)!=SQLITE_OK){executeUnlocked("ROLLBACK;");return Result<void>::failure(sqlite3_errmsg(db_));}sqlite3_bind_text(s,1,r.request_id.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,2,r.topic.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,3,r.shop_domain.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,4,r.customer_id.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,5,r.orders_json.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,6,r.payload_hash.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,7,r.status.c_str(),-1,SQLITE_TRANSIENT);int code=sqlite3_step(s);bool inserted=sqlite3_changes(db_)>0;auto err=code==SQLITE_DONE?std::string{}:std::string(sqlite3_errmsg(db_));sqlite3_finalize(s);if(code!=SQLITE_DONE){executeUnlocked("ROLLBACK;");return Result<void>::failure(err);}if(!inserted){sqlite3_stmt*q=nullptr;if(sqlite3_prepare_v2(db_,"SELECT payload_hash FROM shopify_privacy_requests WHERE request_id=?",-1,&q,nullptr)!=SQLITE_OK){executeUnlocked("ROLLBACK;");return Result<void>::failure(sqlite3_errmsg(db_));}sqlite3_bind_text(q,1,r.request_id.c_str(),-1,SQLITE_TRANSIENT);std::string hash;if(sqlite3_step(q)==SQLITE_ROW)hash=columnText(q,0);sqlite3_finalize(q);if(hash!=r.payload_hash){executeUnlocked("ROLLBACK;");return Result<void>::failure("Shopify privacy request replay payload mismatch");}}return executeUnlocked("COMMIT;");}

Result<void> Database::completeShopifyPrivacyRequest(const std::string& request_id,
                                                            const std::string& status,
                                                            const std::string& result_path,
                                                            const std::string& last_error) {
    if (request_id.empty() || status.empty()) {
        return Result<void>::failure("Privacy request id and status are required");
    }
    return prepareAndStep(
        "UPDATE shopify_privacy_requests SET status=?,result_path=?,last_error=?,"
        "completed_at=CASE WHEN ? IN ('completed','redacted','failed') THEN CURRENT_TIMESTAMP ELSE completed_at END,"
        "updated_at=CURRENT_TIMESTAMP WHERE request_id=?",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, status.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, result_path.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 3, last_error.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 4, status.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 5, request_id.c_str(), -1, SQLITE_TRANSIENT);
        });
}

Result<void> Database::setRuntimeState(const std::string& key, const std::string& value_json) {
    return prepareAndStep(
        "INSERT INTO runtime_state(key,value_json,updated_at) VALUES(?,?,?) "
        "ON CONFLICT(key) DO UPDATE SET value_json=excluded.value_json,updated_at=excluded.updated_at",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, key.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, value_json.c_str(), -1, SQLITE_TRANSIENT);
            const auto now = util::utcNowIso();
            sqlite3_bind_text(statement, 3, now.c_str(), -1, SQLITE_TRANSIENT);
        });
}

Result<std::optional<std::string>> Database::runtimeState(const std::string& key) {
    std::lock_guard lock(mutex_);
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(db_, "SELECT value_json FROM runtime_state WHERE key=?", -1, &statement, nullptr) != SQLITE_OK) {
        return Result<std::optional<std::string>>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(statement, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    std::optional<std::string> output;
    if (sqlite3_step(statement) == SQLITE_ROW) output = columnText(statement, 0);
    sqlite3_finalize(statement);
    return Result<std::optional<std::string>>::success(output);
}

Result<void> Database::setSyncCheckpoint(const std::string& name,
                                               const std::string& cursor,
                                               const std::string& metadata_json,
                                               bool completed) {
    if (name.empty()) return Result<void>::failure("Checkpoint name is required");
    auto parsed = Json::parse(metadata_json.empty() ? "{}" : metadata_json);
    if (!parsed || !parsed.value().isObject()) {
        return Result<void>::failure("Checkpoint metadata must be a JSON object");
    }
    return prepareAndStep(
        "INSERT INTO shopify_sync_checkpoints(name,cursor,metadata_json,completed,updated_at) "
        "VALUES(?,?,?,?,?) ON CONFLICT(name) DO UPDATE SET cursor=excluded.cursor,"
        "metadata_json=excluded.metadata_json,completed=excluded.completed,updated_at=excluded.updated_at",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, cursor.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 3, metadata_json.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(statement, 4, completed ? 1 : 0);
            const auto now = util::utcNowIso();
            sqlite3_bind_text(statement, 5, now.c_str(), -1, SQLITE_TRANSIENT);
        });
}

Result<std::optional<std::string>> Database::syncCheckpoint(const std::string& name) {
    std::lock_guard lock(mutex_);
    if (!db_) return Result<std::optional<std::string>>::failure("Database is not open");
    sqlite3_stmt* statement = nullptr;
    const char* sql =
        "SELECT json_object('name',name,'cursor',cursor,'metadata',json(metadata_json),"
        "'completed',completed,'updated_at',updated_at) FROM shopify_sync_checkpoints WHERE name=?";
    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        return Result<std::optional<std::string>>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(statement, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    const int code = sqlite3_step(statement);
    std::optional<std::string> output;
    if (code == SQLITE_ROW) output = columnText(statement, 0);
    const std::string error = (code == SQLITE_ROW || code == SQLITE_DONE)
        ? std::string{} : std::string(sqlite3_errmsg(db_));
    sqlite3_finalize(statement);
    if (!error.empty()) return Result<std::optional<std::string>>::failure(error);
    return Result<std::optional<std::string>>::success(std::move(output));
}

Result<void> Database::audit(const std::string& level,
                             const std::string& category,
                             const std::string& message,
                             const std::string& context,
                             const std::string& correlation_id) {
    std::string previous_hash;
    {
        std::lock_guard lock(mutex_);
        sqlite3_stmt* query = nullptr;
        if (sqlite3_prepare_v2(db_, "SELECT COALESCE(entry_hash,'') FROM audit_log ORDER BY id DESC LIMIT 1", -1, &query, nullptr) == SQLITE_OK) {
            if (sqlite3_step(query) == SQLITE_ROW) previous_hash = columnText(query, 0);
        }
        sqlite3_finalize(query);
    }
    const auto now = util::utcNowIso();
    const auto entry_hash = crypto::sha256Hex(previous_hash + level + category + message + context + correlation_id + now);
    return prepareAndStep(
        "INSERT INTO audit_log(level,category,message,correlation_id,context_json,previous_hash,entry_hash,created_at) "
        "VALUES(?,?,?,?,?,?,?,?)",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, level.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, category.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 3, message.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 4, correlation_id.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 5, context.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 6, previous_hash.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 7, entry_hash.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 8, now.c_str(), -1, SQLITE_TRANSIENT);
        });
}

Result<void> Database::metric(const std::string& name, double value, const std::string& labels_json) {
    return prepareAndStep(
        "INSERT INTO metrics(name,value,labels_json,updated_at) VALUES(?,?,?,?) "
        "ON CONFLICT(name) DO UPDATE SET value=excluded.value,labels_json=excluded.labels_json,"
        "updated_at=excluded.updated_at",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(statement, 2, value);
            sqlite3_bind_text(statement, 3, labels_json.c_str(), -1, SQLITE_TRANSIENT);
            const auto now = util::utcNowIso();
            sqlite3_bind_text(statement, 4, now.c_str(), -1, SQLITE_TRANSIENT);
        });
}


Result<ShopifyIdempotencyReservation> Database::reserveShopifyIdempotency(
    const std::string& idempotency_key,
    const std::string& operation_name,
    const std::string& parameter_hash,
    int ttl_hours) {
    if (idempotency_key.empty() || operation_name.empty() || parameter_hash.empty()) {
        return Result<ShopifyIdempotencyReservation>::failure(
            "Shopify idempotency key, operation and parameter hash are required");
    }
    ttl_hours = std::clamp(ttl_hours, 1, 72);
    std::lock_guard lock(mutex_);
    if (!db_) return Result<ShopifyIdempotencyReservation>::failure("Database is not open");
    auto begun = executeUnlocked("BEGIN IMMEDIATE;");
    if (!begun) return Result<ShopifyIdempotencyReservation>::failure(begun.error());

    const std::string interval = "+" + std::to_string(ttl_hours) + " hours";
    sqlite3_stmt* insert = nullptr;
    const char* insert_sql =
        "INSERT OR IGNORE INTO shopify_idempotency_keys("
        "idempotency_key,operation_name,parameter_hash,status,created_at,updated_at,expires_at)"
        " VALUES(?,?,?,'reserved',CURRENT_TIMESTAMP,CURRENT_TIMESTAMP,datetime('now',?))";
    if (sqlite3_prepare_v2(db_, insert_sql, -1, &insert, nullptr) != SQLITE_OK) {
        executeUnlocked("ROLLBACK;");
        return Result<ShopifyIdempotencyReservation>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(insert, 1, idempotency_key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insert, 2, operation_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insert, 3, parameter_hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insert, 4, interval.c_str(), -1, SQLITE_TRANSIENT);
    const int insert_code = sqlite3_step(insert);
    bool inserted = sqlite3_changes(db_) > 0;
    const std::string insert_error =
        insert_code == SQLITE_DONE ? std::string{} : std::string(sqlite3_errmsg(db_));
    sqlite3_finalize(insert);
    if (insert_code != SQLITE_DONE) {
        executeUnlocked("ROLLBACK;");
        return Result<ShopifyIdempotencyReservation>::failure(insert_error);
    }

    sqlite3_stmt* query = nullptr;
    const char* query_sql =
        "SELECT operation_name,parameter_hash,status,response_json,last_error,expires_at,"
        "CASE WHEN datetime(expires_at)<=datetime('now') THEN 1 ELSE 0 END "
        "FROM shopify_idempotency_keys WHERE idempotency_key=?";
    if (sqlite3_prepare_v2(db_, query_sql, -1, &query, nullptr) != SQLITE_OK) {
        executeUnlocked("ROLLBACK;");
        return Result<ShopifyIdempotencyReservation>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(query, 1, idempotency_key.c_str(), -1, SQLITE_TRANSIENT);
    ShopifyIdempotencyReservation output;
    output.idempotency_key = idempotency_key;
    bool expired = false;
    if (sqlite3_step(query) == SQLITE_ROW) {
        output.operation_name = columnText(query, 0);
        output.parameter_hash = columnText(query, 1);
        output.status = columnText(query, 2);
        output.response_json = columnText(query, 3);
        output.last_error = columnText(query, 4);
        output.expires_at = columnText(query, 5);
        expired = sqlite3_column_int(query, 6) != 0;
    } else {
        sqlite3_finalize(query);
        executeUnlocked("ROLLBACK;");
        return Result<ShopifyIdempotencyReservation>::failure(
            "Reserved Shopify idempotency row could not be loaded");
    }
    sqlite3_finalize(query);

    if (!inserted && expired) {
        sqlite3_stmt* reset = nullptr;
        const char* reset_sql =
            "UPDATE shopify_idempotency_keys SET operation_name=?,parameter_hash=?,"
            "status='reserved',response_json='',last_error='',created_at=CURRENT_TIMESTAMP,"
            "updated_at=CURRENT_TIMESTAMP,expires_at=datetime('now',?) WHERE idempotency_key=?";
        if (sqlite3_prepare_v2(db_, reset_sql, -1, &reset, nullptr) != SQLITE_OK) {
            executeUnlocked("ROLLBACK;");
            return Result<ShopifyIdempotencyReservation>::failure(sqlite3_errmsg(db_));
        }
        sqlite3_bind_text(reset, 1, operation_name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(reset, 2, parameter_hash.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(reset, 3, interval.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(reset, 4, idempotency_key.c_str(), -1, SQLITE_TRANSIENT);
        const int reset_code = sqlite3_step(reset);
        const std::string reset_error =
            reset_code == SQLITE_DONE ? std::string{} : std::string(sqlite3_errmsg(db_));
        sqlite3_finalize(reset);
        if (reset_code != SQLITE_DONE) {
            executeUnlocked("ROLLBACK;");
            return Result<ShopifyIdempotencyReservation>::failure(reset_error);
        }
        inserted = true;
        output.operation_name = operation_name;
        output.parameter_hash = parameter_hash;
        output.status = "reserved";
        output.response_json.clear();
        output.last_error.clear();
    }

    if (!inserted &&
        (output.operation_name != operation_name || output.parameter_hash != parameter_hash)) {
        executeUnlocked("ROLLBACK;");
        return Result<ShopifyIdempotencyReservation>::failure(
            "Shopify idempotency key was reused with different operation parameters");
    }
    output.inserted = inserted;
    auto committed = executeUnlocked("COMMIT;");
    if (!committed) {
        return Result<ShopifyIdempotencyReservation>::failure(committed.error());
    }
    return Result<ShopifyIdempotencyReservation>::success(std::move(output));
}

Result<void> Database::markShopifyIdempotencyInProgress(
    const std::string& idempotency_key) {
    if (idempotency_key.empty()) return Result<void>::failure("Idempotency key is empty");
    return prepareAndStep(
        "UPDATE shopify_idempotency_keys SET status='in_progress',updated_at=CURRENT_TIMESTAMP "
        "WHERE idempotency_key=? AND status IN ('reserved','failed')",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, idempotency_key.c_str(), -1, SQLITE_TRANSIENT);
        });
}

Result<void> Database::completeShopifyIdempotency(
    const std::string& idempotency_key,
    const std::string& response_json) {
    if (idempotency_key.empty()) return Result<void>::failure("Idempotency key is empty");
    return prepareAndStep(
        "UPDATE shopify_idempotency_keys SET status='completed',response_json=?,last_error='',"
        "updated_at=CURRENT_TIMESTAMP WHERE idempotency_key=?",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, response_json.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, idempotency_key.c_str(), -1, SQLITE_TRANSIENT);
        });
}

Result<void> Database::failShopifyIdempotency(
    const std::string& idempotency_key,
    const std::string& error) {
    if (idempotency_key.empty()) return Result<void>::failure("Idempotency key is empty");
    return prepareAndStep(
        "UPDATE shopify_idempotency_keys SET status='failed',last_error=?,"
        "updated_at=CURRENT_TIMESTAMP WHERE idempotency_key=?",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, error.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, idempotency_key.c_str(), -1, SQLITE_TRANSIENT);
        });
}

Result<ShopifyWebhookOrderResult> Database::evaluateShopifyWebhookOrder(
    const std::string& shop_domain,
    const std::string& resource_type,
    const std::string& resource_id,
    const std::string& triggered_at,
    const std::string& event_id,
    const std::string& webhook_id,
    const std::string& payload_hash) {
    ShopifyWebhookOrderResult output;
    std::string normalized_triggered_at = triggered_at;
    if (!normalized_triggered_at.empty()) {
        auto normalized = shopify::ShopifyTimestamp::normalizeUtc(triggered_at);
        if (!normalized) return Result<ShopifyWebhookOrderResult>::failure(normalized.error());
        normalized_triggered_at = normalized.take();
    }
    if (shop_domain.empty() || resource_type.empty() || resource_id.empty()) {
        output.reason = "resource_not_orderable";
        return Result<ShopifyWebhookOrderResult>::success(std::move(output));
    }
    std::lock_guard lock(mutex_);
    if (!db_) return Result<ShopifyWebhookOrderResult>::failure("Database is not open");
    auto begun = executeUnlocked("BEGIN IMMEDIATE;");
    if (!begun) return Result<ShopifyWebhookOrderResult>::failure(begun.error());

    sqlite3_stmt* query = nullptr;
    const char* query_sql =
        "SELECT last_triggered_at,last_event_id,last_webhook_id,last_payload_hash "
        "FROM shopify_webhook_ordering WHERE shop_domain=? AND resource_type=? AND resource_id=?";
    if (sqlite3_prepare_v2(db_, query_sql, -1, &query, nullptr) != SQLITE_OK) {
        executeUnlocked("ROLLBACK;");
        return Result<ShopifyWebhookOrderResult>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(query, 1, shop_domain.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(query, 2, resource_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(query, 3, resource_id.c_str(), -1, SQLITE_TRANSIENT);
    std::string previous_webhook;
    std::string previous_hash;
    if (sqlite3_step(query) == SQLITE_ROW) {
        output.previous_triggered_at = columnText(query, 0);
        output.previous_event_id = columnText(query, 1);
        previous_webhook = columnText(query, 2);
        previous_hash = columnText(query, 3);
    }
    sqlite3_finalize(query);

    if ((!webhook_id.empty() && webhook_id == previous_webhook) ||
        (!event_id.empty() && event_id == output.previous_event_id) ||
        (!payload_hash.empty() && payload_hash == previous_hash &&
         !normalized_triggered_at.empty() && normalized_triggered_at == output.previous_triggered_at)) {
        output.decision = ShopifyWebhookOrderDecision::duplicate;
        output.reason = "duplicate_event";
    } else if (!normalized_triggered_at.empty() && !output.previous_triggered_at.empty() &&
               normalized_triggered_at < output.previous_triggered_at) {
        output.decision = ShopifyWebhookOrderDecision::stale;
        output.reason = "triggered_at_older_than_last_accepted";
    } else {
        output.decision = ShopifyWebhookOrderDecision::accepted;
        output.reason = "accepted";
    }

    const char* upsert_sql =
        "INSERT INTO shopify_webhook_ordering("
        "shop_domain,resource_type,resource_id,last_triggered_at,last_event_id,last_webhook_id,"
        "last_payload_hash,accepted_count,stale_count,duplicate_count,updated_at)"
        " VALUES(?,?,?,?,?,?,?, ?,?,?,CURRENT_TIMESTAMP)"
        " ON CONFLICT(shop_domain,resource_type,resource_id) DO UPDATE SET "
        "last_triggered_at=CASE WHEN excluded.accepted_count=1 AND excluded.last_triggered_at<>'' "
        "THEN excluded.last_triggered_at ELSE shopify_webhook_ordering.last_triggered_at END,"
        "last_event_id=CASE WHEN excluded.accepted_count=1 AND excluded.last_event_id<>'' "
        "THEN excluded.last_event_id ELSE shopify_webhook_ordering.last_event_id END,"
        "last_webhook_id=CASE WHEN excluded.accepted_count=1 AND excluded.last_webhook_id<>'' "
        "THEN excluded.last_webhook_id ELSE shopify_webhook_ordering.last_webhook_id END,"
        "last_payload_hash=CASE WHEN excluded.accepted_count=1 AND excluded.last_payload_hash<>'' "
        "THEN excluded.last_payload_hash ELSE shopify_webhook_ordering.last_payload_hash END,"
        "accepted_count=shopify_webhook_ordering.accepted_count+excluded.accepted_count,"
        "stale_count=shopify_webhook_ordering.stale_count+excluded.stale_count,"
        "duplicate_count=shopify_webhook_ordering.duplicate_count+excluded.duplicate_count,"
        "updated_at=CURRENT_TIMESTAMP";
    sqlite3_stmt* upsert = nullptr;
    if (sqlite3_prepare_v2(db_, upsert_sql, -1, &upsert, nullptr) != SQLITE_OK) {
        executeUnlocked("ROLLBACK;");
        return Result<ShopifyWebhookOrderResult>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(upsert, 1, shop_domain.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(upsert, 2, resource_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(upsert, 3, resource_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(upsert, 4, normalized_triggered_at.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(upsert, 5, event_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(upsert, 6, webhook_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(upsert, 7, payload_hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(upsert, 8, output.decision == ShopifyWebhookOrderDecision::accepted ? 1 : 0);
    sqlite3_bind_int(upsert, 9, output.decision == ShopifyWebhookOrderDecision::stale ? 1 : 0);
    sqlite3_bind_int(upsert, 10, output.decision == ShopifyWebhookOrderDecision::duplicate ? 1 : 0);
    const int code = sqlite3_step(upsert);
    const std::string error = code == SQLITE_DONE ? std::string{} : std::string(sqlite3_errmsg(db_));
    sqlite3_finalize(upsert);
    if (code != SQLITE_DONE) {
        executeUnlocked("ROLLBACK;");
        return Result<ShopifyWebhookOrderResult>::failure(error);
    }
    auto committed = executeUnlocked("COMMIT;");
    if (!committed) return Result<ShopifyWebhookOrderResult>::failure(committed.error());
    return Result<ShopifyWebhookOrderResult>::success(std::move(output));
}

Result<TaskEnqueueResult> Database::requestShopifyReconciliation(
    const std::string& shop_domain,
    const std::string& resource_type,
    const std::string& resource_id,
    const std::string& reason,
    const std::string& source_event_id,
    const std::string& source_webhook_id,
    const std::string& requested_from) {
    if (shop_domain.empty() || resource_type.empty() || reason.empty()) {
        return Result<TaskEnqueueResult>::failure(
            "Shopify reconciliation shop, resource type and reason are required");
    }
    Json payload = Json::object();
    payload.set("shop_domain", shop_domain);
    payload.set("resource_type", resource_type);
    payload.set("resource_id", resource_id);
    payload.set("reason", reason);
    payload.set("source_event_id", source_event_id);
    payload.set("source_webhook_id", source_webhook_id);
    payload.set("requested_from", requested_from);
    const auto source_identity = !source_event_id.empty() ? source_event_id
        : (!source_webhook_id.empty() ? source_webhook_id
                                      : util::utcNowIso().substr(0, 13));
    const auto dedupe_key = "shopify-reconcile:" +
        crypto::sha256Hex(shop_domain + "\n" + resource_type + "\n" + resource_id +
                          "\n" + reason + "\n" + source_identity);
    payload.set("dedupe_key", dedupe_key);

    auto row = prepareAndStep(
        "INSERT OR IGNORE INTO shopify_reconciliation_requests("
        "shop_domain,resource_type,resource_id,reason,source_event_id,source_webhook_id,"
        "requested_from,dedupe_key) VALUES(?,?,?,?,?,?,?,?)",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, shop_domain.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, resource_type.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 3, resource_id.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 4, reason.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 5, source_event_id.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 6, source_webhook_id.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 7, requested_from.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 8, dedupe_key.c_str(), -1, SQLITE_TRANSIENT);
        });
    if (!row) return Result<TaskEnqueueResult>::failure(row.error());
    return enqueueUniqueTask("shopify_reconcile_resource", payload.dump(), 5, {}, dedupe_key);
}

Result<void> Database::updateShopifyReconciliationStatus(
    const std::string& dedupe_key,
    const std::string& status,
    const std::string& last_error) {
    static const std::array<const char*, 5> allowed{
        "pending", "running", "completed", "failed", "cancelled"};
    if (dedupe_key.empty()) {
        return Result<void>::failure("Shopify reconciliation dedupe key is empty");
    }
    if (std::find(allowed.begin(), allowed.end(), status) == allowed.end()) {
        return Result<void>::failure("Invalid Shopify reconciliation status: " + status);
    }
    return prepareAndStep(
        "UPDATE shopify_reconciliation_requests SET status=?,"
        "attempts=CASE WHEN ?='running' THEN attempts+1 ELSE attempts END,"
        "last_error=?,updated_at=CURRENT_TIMESTAMP,"
        "completed_at=CASE WHEN ? IN ('completed','cancelled') THEN CURRENT_TIMESTAMP ELSE NULL END "
        "WHERE dedupe_key=?",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, status.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, status.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 3, last_error.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 4, status.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 5, dedupe_key.c_str(), -1, SQLITE_TRANSIENT);
        });
}



Result<void> Database::recordShopifyGovernanceRun(
    const std::string& shop_domain,
    const std::string& mode,
    int score,
    bool production_ready,
    const std::string& api_version,
    const std::string& report_json) {
    if (mode != "offline" && mode != "live") {
        return Result<void>::failure("Shopify governance mode must be offline or live");
    }
    if (score < 0 || score > 100) {
        return Result<void>::failure("Shopify governance score must be between 0 and 100");
    }
    auto parsed = Json::parse(report_json);
    if (!parsed || !parsed.value().isObject()) {
        return Result<void>::failure("Shopify governance report must be a valid JSON object");
    }
    return prepareAndStep(
        "INSERT INTO shopify_governance_runs("
        "shop_domain,mode,score,production_ready,api_version,report_json) VALUES(?,?,?,?,?,?)",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, shop_domain.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, mode.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(statement, 3, score);
            sqlite3_bind_int(statement, 4, production_ready ? 1 : 0);
            sqlite3_bind_text(statement, 5, api_version.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 6, report_json.c_str(), -1, SQLITE_TRANSIENT);
        });
}

Result<std::optional<std::string>> Database::latestShopifyGovernanceRun(
    const std::string& shop_domain) {
    std::lock_guard lock(mutex_);
    if (!db_) return Result<std::optional<std::string>>::failure("Database is not open");
    sqlite3_stmt* statement = nullptr;
    const char* sql =
        "SELECT report_json FROM shopify_governance_runs WHERE shop_domain=? "
        "ORDER BY created_at DESC,id DESC LIMIT 1";
    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        return Result<std::optional<std::string>>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(statement, 1, shop_domain.c_str(), -1, SQLITE_TRANSIENT);
    std::optional<std::string> output;
    const int code = sqlite3_step(statement);
    if (code == SQLITE_ROW) output = columnText(statement, 0);
    const std::string error = code == SQLITE_ROW || code == SQLITE_DONE
        ? std::string{} : std::string(sqlite3_errmsg(db_));
    sqlite3_finalize(statement);
    if (!error.empty()) return Result<std::optional<std::string>>::failure(error);
    return Result<std::optional<std::string>>::success(std::move(output));
}

Result<void> Database::recordShopifyApiVersionObservation(
    const std::string& shop_domain,
    const std::string& configured_version,
    const std::string& latest_supported_version,
    bool supported,
    const std::string& catalog_json) {
    auto parsed = Json::parse(catalog_json);
    if (!parsed || !parsed.value().isObject()) {
        return Result<void>::failure("Shopify API version catalog must be a valid JSON object");
    }
    return prepareAndStep(
        "INSERT INTO shopify_api_catalog_observations("
        "shop_domain,configured_version,latest_supported_version,supported,catalog_json) "
        "VALUES(?,?,?,?,?)",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, shop_domain.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, configured_version.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 3, latest_supported_version.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(statement, 4, supported ? 1 : 0);
            sqlite3_bind_text(statement, 5, catalog_json.c_str(), -1, SQLITE_TRANSIENT);
        });
}


Result<void> Database::recordShopifyContractAudit(
    int score,
    bool passed,
    const std::string& report_json) {
    if (score < 0 || score > 100) {
        return Result<void>::failure("Shopify contract audit score must be between 0 and 100");
    }
    auto parsed = Json::parse(report_json);
    if (!parsed || !parsed.value().isObject()) {
        return Result<void>::failure("Shopify contract audit report must be valid JSON");
    }
    return prepareAndStep(
        "INSERT INTO shopify_api_contract_audits(score,passed,report_json) VALUES(?,?,?)",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_int(statement, 1, score);
            sqlite3_bind_int(statement, 2, passed ? 1 : 0);
            sqlite3_bind_text(statement, 3, report_json.c_str(), -1, SQLITE_TRANSIENT);
        });
}

Result<void> Database::recordShopifyCatalogSync(
    const std::string& external_product_id,
    const std::string& mode,
    const std::string& status,
    const std::string& product_gid,
    const std::string& operation_gid,
    const std::string& result_json,
    const std::string& last_error) {
    if (external_product_id.empty()) {
        return Result<void>::failure("Shopify catalog sync external product ID is empty");
    }
    if (mode != "synchronous" && mode != "asynchronous") {
        return Result<void>::failure("Shopify catalog sync mode is invalid");
    }
    auto parsed = Json::parse(result_json);
    if (!parsed || !parsed.value().isObject()) {
        return Result<void>::failure("Shopify catalog sync result must be valid JSON");
    }
    return prepareAndStep(
        "INSERT INTO shopify_catalog_sync_runs("
        "external_product_id,mode,status,product_gid,operation_gid,result_json,last_error) "
        "VALUES(?,?,?,?,?,?,?)",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, external_product_id.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, mode.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 3, status.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 4, product_gid.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 5, operation_gid.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 6, result_json.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 7, last_error.c_str(), -1, SQLITE_TRANSIENT);
        });
}


Result<void> Database::recordShopifyWebhookSloAudit(
    std::size_t deliveries,
    double failure_rate_percent,
    double p90_response_time_ms,
    std::size_t removed_subscriptions,
    bool healthy,
    const std::string& report_json) {
    if (!std::isfinite(failure_rate_percent) || failure_rate_percent < 0.0 ||
        !std::isfinite(p90_response_time_ms) || p90_response_time_ms < 0.0) {
        return Result<void>::failure("Invalid Shopify webhook SLO metrics");
    }
    auto parsed = Json::parse(report_json);
    if (!parsed || !parsed.value().isObject()) {
        return Result<void>::failure("Shopify webhook SLO report must be valid JSON");
    }
    return prepareAndStep(
        "INSERT INTO shopify_webhook_slo_audits("
        "deliveries,failure_rate_percent,p90_response_time_ms,removed_subscriptions,healthy,report_json) "
        "VALUES(?,?,?,?,?,?)",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_int64(statement, 1, static_cast<sqlite3_int64>(deliveries));
            sqlite3_bind_double(statement, 2, failure_rate_percent);
            sqlite3_bind_double(statement, 3, p90_response_time_ms);
            sqlite3_bind_int64(statement, 4, static_cast<sqlite3_int64>(removed_subscriptions));
            sqlite3_bind_int(statement, 5, healthy ? 1 : 0);
            sqlite3_bind_text(statement, 6, report_json.c_str(), -1, SQLITE_TRANSIENT);
        });
}



} // namespace elit21
