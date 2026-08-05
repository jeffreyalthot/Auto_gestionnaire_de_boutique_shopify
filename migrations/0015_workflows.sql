CREATE TABLE IF NOT EXISTS workflows(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  workflow_type TEXT NOT NULL,
  correlation_id TEXT NOT NULL UNIQUE,
  state TEXT NOT NULL,
  checkpoint_json TEXT NOT NULL DEFAULT '{}',
  attempt_count INTEGER NOT NULL DEFAULT 0,
  last_error TEXT,
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
CREATE INDEX IF NOT EXISTS idx_workflows_state ON workflows(state);
