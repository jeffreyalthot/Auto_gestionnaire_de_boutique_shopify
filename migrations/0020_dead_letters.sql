CREATE TABLE IF NOT EXISTS dead_letters(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  source_table TEXT NOT NULL,
  source_id INTEGER,
  category TEXT NOT NULL,
  payload_json TEXT NOT NULL,
  error_code TEXT,
  error_message TEXT,
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  resolved_at TEXT
);
CREATE INDEX IF NOT EXISTS idx_dead_letters_unresolved ON dead_letters(resolved_at, created_at);
