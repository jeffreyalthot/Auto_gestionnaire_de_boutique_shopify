CREATE TABLE IF NOT EXISTS products(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  aliexpress_product_id TEXT NOT NULL UNIQUE,
  shopify_product_id TEXT,
  title TEXT NOT NULL,
  supplier_cost_cad REAL NOT NULL CHECK(supplier_cost_cad>=0),
  shipping_cost_cad REAL NOT NULL DEFAULT 0 CHECK(shipping_cost_cad>=0),
  sale_price_cad REAL NOT NULL CHECK(sale_price_cad>=0),
  stock INTEGER NOT NULL DEFAULT 0 CHECK(stock>=0),
  status TEXT NOT NULL DEFAULT 'candidate',
  payload_json TEXT NOT NULL DEFAULT '{}',
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
CREATE INDEX IF NOT EXISTS idx_products_status ON products(status);
CREATE INDEX IF NOT EXISTS idx_products_shopify_id ON products(shopify_product_id);
