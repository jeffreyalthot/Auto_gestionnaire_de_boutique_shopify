CREATE TABLE IF NOT EXISTS shopify_api_contract_audits (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    score INTEGER NOT NULL CHECK(score BETWEEN 0 AND 100),
    passed INTEGER NOT NULL CHECK(passed IN (0,1)),
    report_json TEXT NOT NULL CHECK(json_valid(report_json)),
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_shopify_api_contract_audits_created
    ON shopify_api_contract_audits(created_at DESC, id DESC);

CREATE TABLE IF NOT EXISTS shopify_catalog_sync_runs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    external_product_id TEXT NOT NULL,
    mode TEXT NOT NULL CHECK(mode IN ('synchronous','asynchronous')),
    status TEXT NOT NULL,
    product_gid TEXT NOT NULL DEFAULT '',
    operation_gid TEXT NOT NULL DEFAULT '',
    result_json TEXT NOT NULL CHECK(json_valid(result_json)),
    last_error TEXT NOT NULL DEFAULT '',
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_shopify_catalog_sync_external_created
    ON shopify_catalog_sync_runs(external_product_id, created_at DESC, id DESC);
CREATE INDEX IF NOT EXISTS idx_shopify_catalog_sync_operation
    ON shopify_catalog_sync_runs(operation_gid) WHERE operation_gid <> '';


CREATE TABLE IF NOT EXISTS shopify_webhook_slo_audits (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    deliveries INTEGER NOT NULL CHECK(deliveries >= 0),
    failure_rate_percent REAL NOT NULL CHECK(failure_rate_percent >= 0),
    p90_response_time_ms REAL NOT NULL CHECK(p90_response_time_ms >= 0),
    removed_subscriptions INTEGER NOT NULL CHECK(removed_subscriptions >= 0),
    healthy INTEGER NOT NULL CHECK(healthy IN (0,1)),
    report_json TEXT NOT NULL CHECK(json_valid(report_json)),
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_shopify_webhook_slo_created
    ON shopify_webhook_slo_audits(created_at DESC, id DESC);
