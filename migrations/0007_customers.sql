CREATE TABLE IF NOT EXISTS customers(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  shopify_customer_id TEXT UNIQUE,
  email_hash TEXT,
  locale TEXT NOT NULL DEFAULT 'fr-CA',
  consent_status TEXT NOT NULL DEFAULT 'unknown',
  risk_status TEXT NOT NULL DEFAULT 'normal',
  payload_json TEXT NOT NULL DEFAULT '{}',
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
