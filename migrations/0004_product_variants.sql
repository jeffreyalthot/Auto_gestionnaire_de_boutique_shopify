CREATE TABLE IF NOT EXISTS variants(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  product_id INTEGER NOT NULL REFERENCES products(id) ON DELETE CASCADE,
  aliexpress_sku_id TEXT NOT NULL,
  shopify_variant_id TEXT,
  shopify_inventory_item_id TEXT,
  sku TEXT NOT NULL,
  supplier_cost_cad REAL NOT NULL CHECK(supplier_cost_cad>=0),
  shipping_cost_cad REAL NOT NULL DEFAULT 0 CHECK(shipping_cost_cad>=0),
  sale_price_cad REAL NOT NULL CHECK(sale_price_cad>=0),
  stock INTEGER NOT NULL DEFAULT 0 CHECK(stock>=0),
  status TEXT NOT NULL DEFAULT 'active',
  payload_json TEXT NOT NULL DEFAULT '{}',
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  UNIQUE(product_id, aliexpress_sku_id),
  UNIQUE(sku)
);
CREATE INDEX IF NOT EXISTS idx_variants_shopify_variant ON variants(shopify_variant_id);
