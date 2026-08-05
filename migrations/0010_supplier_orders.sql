CREATE TABLE IF NOT EXISTS supplier_orders(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  order_id INTEGER NOT NULL REFERENCES orders(id) ON DELETE CASCADE,
  aliexpress_order_id TEXT UNIQUE,
  shipping_service TEXT,
  amount_cad REAL NOT NULL DEFAULT 0 CHECK(amount_cad>=0),
  status TEXT NOT NULL,
  request_json TEXT NOT NULL DEFAULT '{}',
  response_json TEXT NOT NULL DEFAULT '{}',
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
