CREATE TABLE IF NOT EXISTS price_history(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  variant_id INTEGER REFERENCES variants(id) ON DELETE CASCADE,
  supplier_cost_cad REAL NOT NULL,
  shipping_cost_cad REAL NOT NULL,
  price_before_shipping_cad REAL NOT NULL,
  final_price_cad REAL NOT NULL,
  markup_percent REAL NOT NULL CHECK(markup_percent>=100),
  observed_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
CREATE INDEX IF NOT EXISTS idx_price_history_variant ON price_history(variant_id, observed_at);
