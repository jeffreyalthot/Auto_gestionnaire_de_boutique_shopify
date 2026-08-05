CREATE TABLE IF NOT EXISTS webhooks(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  provider TEXT NOT NULL,
  webhook_id TEXT NOT NULL,
  topic TEXT NOT NULL,
  payload_hash TEXT NOT NULL,
  status TEXT NOT NULL DEFAULT 'received',
  received_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  processed_at TEXT,
  last_error TEXT,
  UNIQUE(provider, webhook_id)
);
CREATE INDEX IF NOT EXISTS idx_webhooks_status ON webhooks(status);
