CREATE TABLE IF NOT EXISTS metrics(
  name TEXT PRIMARY KEY,
  value REAL NOT NULL,
  labels_json TEXT NOT NULL DEFAULT '{}',
  updated_at TEXT NOT NULL
);
