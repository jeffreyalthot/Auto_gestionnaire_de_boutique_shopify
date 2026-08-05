CREATE TABLE IF NOT EXISTS orders(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  shopify_order_id TEXT NOT NULL UNIQUE,
  aliexpress_order_id TEXT,
  customer_id INTEGER REFERENCES customers(id) ON DELETE SET NULL,
  customer_email TEXT,
  total_cad REAL NOT NULL DEFAULT 0 CHECK(total_cad>=0),
  currency TEXT NOT NULL DEFAULT 'CAD',
  status TEXT NOT NULL,
  payload_json TEXT NOT NULL,
  idempotency_key TEXT NOT NULL UNIQUE,
  attempt_count INTEGER NOT NULL DEFAULT 0,
  last_error TEXT,
  created_at TEXT NOT NULL,
  updated_at TEXT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_orders_status ON orders(status);
CREATE INDEX IF NOT EXISTS idx_orders_supplier_id ON orders(aliexpress_order_id);
