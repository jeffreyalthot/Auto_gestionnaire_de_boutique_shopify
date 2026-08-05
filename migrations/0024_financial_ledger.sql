CREATE TABLE IF NOT EXISTS financial_ledger(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  order_id INTEGER REFERENCES orders(id) ON DELETE SET NULL,
  entry_type TEXT NOT NULL,
  amount_cad REAL NOT NULL,
  reference_id TEXT,
  description TEXT,
  occurred_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
CREATE INDEX IF NOT EXISTS idx_ledger_order ON financial_ledger(order_id);
