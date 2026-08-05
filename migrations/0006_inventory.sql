CREATE TABLE IF NOT EXISTS inventory_levels(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  variant_id INTEGER REFERENCES variants(id) ON DELETE CASCADE,
  supplier_stock INTEGER NOT NULL DEFAULT 0 CHECK(supplier_stock>=0),
  safety_buffer INTEGER NOT NULL DEFAULT 0 CHECK(safety_buffer>=0),
  available_to_sell INTEGER NOT NULL DEFAULT 0 CHECK(available_to_sell>=0),
  shopify_quantity INTEGER NOT NULL DEFAULT 0 CHECK(shopify_quantity>=0),
  observed_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  UNIQUE(variant_id)
);
