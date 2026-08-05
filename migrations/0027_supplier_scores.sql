CREATE TABLE IF NOT EXISTS supplier_scores(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  supplier_id TEXT NOT NULL,
  score REAL NOT NULL CHECK(score>=0 AND score<=100),
  delivery_score REAL NOT NULL DEFAULT 0,
  quality_score REAL NOT NULL DEFAULT 0,
  dispute_score REAL NOT NULL DEFAULT 0,
  sample_size INTEGER NOT NULL DEFAULT 0,
  calculated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  UNIQUE(supplier_id)
);
