CREATE TABLE IF NOT EXISTS order_lines(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  order_id INTEGER NOT NULL REFERENCES orders(id) ON DELETE CASCADE,
  shopify_line_item_id TEXT,
  shopify_variant_id TEXT,
  aliexpress_product_id TEXT NOT NULL,
  aliexpress_sku_id TEXT NOT NULL,
  sku TEXT,
  title TEXT,
  quantity INTEGER NOT NULL CHECK(quantity>0),
  unit_price_cad REAL NOT NULL DEFAULT 0 CHECK(unit_price_cad>=0),
  status TEXT NOT NULL DEFAULT 'pending'
);
CREATE INDEX IF NOT EXISTS idx_order_lines_order ON order_lines(order_id);
