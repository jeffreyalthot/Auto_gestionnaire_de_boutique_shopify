CREATE TABLE IF NOT EXISTS reports(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  report_type TEXT NOT NULL,
  period_start TEXT,
  period_end TEXT,
  path TEXT,
  summary_json TEXT NOT NULL DEFAULT '{}',
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
