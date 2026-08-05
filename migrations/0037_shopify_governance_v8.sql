CREATE TABLE IF NOT EXISTS shopify_governance_runs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    shop_domain TEXT NOT NULL DEFAULT '',
    mode TEXT NOT NULL CHECK(mode IN ('offline','live')),
    score INTEGER NOT NULL CHECK(score BETWEEN 0 AND 100),
    production_ready INTEGER NOT NULL DEFAULT 0 CHECK(production_ready IN (0,1)),
    api_version TEXT NOT NULL DEFAULT '',
    report_json TEXT NOT NULL CHECK(json_valid(report_json)),
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_shopify_governance_runs_shop_created
    ON shopify_governance_runs(shop_domain, created_at DESC, id DESC);

CREATE TABLE IF NOT EXISTS shopify_api_catalog_observations (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    shop_domain TEXT NOT NULL DEFAULT '',
    configured_version TEXT NOT NULL,
    latest_supported_version TEXT NOT NULL DEFAULT '',
    supported INTEGER NOT NULL DEFAULT 0 CHECK(supported IN (0,1)),
    catalog_json TEXT NOT NULL CHECK(json_valid(catalog_json)),
    observed_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_shopify_api_catalog_observations_shop
    ON shopify_api_catalog_observations(shop_domain, observed_at DESC, id DESC);
