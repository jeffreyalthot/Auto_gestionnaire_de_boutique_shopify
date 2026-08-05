ALTER TABLE variants ADD COLUMN shopify_quantity INTEGER NOT NULL DEFAULT -1;
ALTER TABLE variants ADD COLUMN shopify_quantity_updated_at TEXT;
CREATE INDEX IF NOT EXISTS idx_variants_shopify_inventory_item
  ON variants(shopify_inventory_item_id);

CREATE TABLE IF NOT EXISTS shopify_idempotency_keys(
  idempotency_key TEXT PRIMARY KEY,
  operation_name TEXT NOT NULL,
  parameter_hash TEXT NOT NULL,
  status TEXT NOT NULL DEFAULT 'reserved'
    CHECK(status IN ('reserved','in_progress','completed','failed','expired')),
  response_json TEXT NOT NULL DEFAULT '',
  last_error TEXT NOT NULL DEFAULT '',
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  expires_at TEXT NOT NULL DEFAULT (datetime('now','+24 hours'))
);
CREATE INDEX IF NOT EXISTS idx_shopify_idempotency_status_expiry
  ON shopify_idempotency_keys(status,expires_at);

CREATE TABLE IF NOT EXISTS shopify_webhook_ordering(
  shop_domain TEXT NOT NULL,
  resource_type TEXT NOT NULL,
  resource_id TEXT NOT NULL,
  last_triggered_at TEXT NOT NULL DEFAULT '',
  last_event_id TEXT NOT NULL DEFAULT '',
  last_webhook_id TEXT NOT NULL DEFAULT '',
  last_payload_hash TEXT NOT NULL DEFAULT '',
  accepted_count INTEGER NOT NULL DEFAULT 0,
  stale_count INTEGER NOT NULL DEFAULT 0,
  duplicate_count INTEGER NOT NULL DEFAULT 0,
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY(shop_domain,resource_type,resource_id)
);
CREATE INDEX IF NOT EXISTS idx_shopify_webhook_ordering_updated
  ON shopify_webhook_ordering(updated_at);

CREATE TABLE IF NOT EXISTS shopify_reconciliation_requests(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  shop_domain TEXT NOT NULL,
  resource_type TEXT NOT NULL,
  resource_id TEXT NOT NULL DEFAULT '',
  reason TEXT NOT NULL,
  source_event_id TEXT NOT NULL DEFAULT '',
  source_webhook_id TEXT NOT NULL DEFAULT '',
  requested_from TEXT NOT NULL DEFAULT '',
  status TEXT NOT NULL DEFAULT 'pending'
    CHECK(status IN ('pending','running','completed','failed','cancelled')),
  attempts INTEGER NOT NULL DEFAULT 0,
  last_error TEXT NOT NULL DEFAULT '',
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  completed_at TEXT,
  dedupe_key TEXT NOT NULL UNIQUE
);
CREATE INDEX IF NOT EXISTS idx_shopify_reconciliation_requests_status
  ON shopify_reconciliation_requests(status,created_at);

CREATE TABLE IF NOT EXISTS shopify_api_version_guard(
  shop_domain TEXT PRIMARY KEY,
  configured_version TEXT NOT NULL,
  last_served_version TEXT NOT NULL DEFAULT '',
  mismatch_count INTEGER NOT NULL DEFAULT 0,
  last_checked_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  support_ends_at TEXT NOT NULL DEFAULT '',
  state TEXT NOT NULL DEFAULT 'unknown'
);
