CREATE TABLE IF NOT EXISTS inventory_history(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  variant_id INTEGER REFERENCES variants(id) ON DELETE CASCADE,
  supplier_stock INTEGER NOT NULL,
  available_to_sell INTEGER NOT NULL,
  shopify_quantity INTEGER NOT NULL,
  observed_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
CREATE INDEX IF NOT EXISTS idx_inventory_history_variant ON inventory_history(variant_id, observed_at);
