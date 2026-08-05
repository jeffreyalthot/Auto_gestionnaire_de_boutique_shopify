CREATE TABLE IF NOT EXISTS credentials(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  provider TEXT NOT NULL,
  credential_name TEXT NOT NULL,
  encrypted_value BLOB NOT NULL,
  key_version INTEGER NOT NULL DEFAULT 1,
  expires_at TEXT,
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  UNIQUE(provider, credential_name)
);
CREATE INDEX IF NOT EXISTS idx_credentials_provider ON credentials(provider);
