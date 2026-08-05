CREATE TABLE IF NOT EXISTS shopify_sync_checkpoints(
  name TEXT PRIMARY KEY,
  cursor TEXT NOT NULL DEFAULT '',
  metadata_json TEXT NOT NULL DEFAULT '{}',
  completed INTEGER NOT NULL DEFAULT 0 CHECK(completed IN (0,1)),
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS shopify_reconciliation_runs(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  resource_type TEXT NOT NULL,
  mode TEXT NOT NULL,
  status TEXT NOT NULL,
  started_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  completed_at TEXT,
  scanned_count INTEGER NOT NULL DEFAULT 0,
  created_count INTEGER NOT NULL DEFAULT 0,
  updated_count INTEGER NOT NULL DEFAULT 0,
  rejected_count INTEGER NOT NULL DEFAULT 0,
  error_count INTEGER NOT NULL DEFAULT 0,
  cursor TEXT,
  metadata_json TEXT NOT NULL DEFAULT '{}'
);
CREATE INDEX IF NOT EXISTS idx_shopify_reconciliation_status
  ON shopify_reconciliation_runs(resource_type,status,started_at);

CREATE TABLE IF NOT EXISTS shopify_resource_hashes(
  resource_type TEXT NOT NULL,
  resource_id TEXT NOT NULL,
  payload_hash TEXT NOT NULL,
  source_updated_at TEXT,
  synchronized_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY(resource_type,resource_id)
);

CREATE TABLE IF NOT EXISTS api_request_samples(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  provider TEXT NOT NULL,
  operation TEXT NOT NULL,
  http_status INTEGER NOT NULL DEFAULT 0,
  elapsed_ms INTEGER NOT NULL DEFAULT 0,
  requested_cost REAL NOT NULL DEFAULT 0,
  actual_cost REAL NOT NULL DEFAULT 0,
  available_cost REAL NOT NULL DEFAULT 0,
  retry_count INTEGER NOT NULL DEFAULT 0,
  result TEXT NOT NULL,
  correlation_id TEXT,
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
CREATE INDEX IF NOT EXISTS idx_api_request_samples_provider_created
  ON api_request_samples(provider,created_at);
