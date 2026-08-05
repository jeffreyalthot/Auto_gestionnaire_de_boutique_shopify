-- Legacy compatibility entrypoint. Use Database::migrateDirectory("migrations").
PRAGMA foreign_keys=ON;
CREATE TABLE IF NOT EXISTS schema_migrations(
  version TEXT PRIMARY KEY,
  name TEXT NOT NULL,
  checksum TEXT NOT NULL DEFAULT '',
  applied_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
CREATE TABLE IF NOT EXISTS runtime_state(
  key TEXT PRIMARY KEY,
  value_json TEXT NOT NULL DEFAULT '{}',
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
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
CREATE TABLE IF NOT EXISTS shipments(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  order_id INTEGER NOT NULL REFERENCES orders(id) ON DELETE CASCADE,
  fulfillment_order_id TEXT,
  shopify_fulfillment_id TEXT,
  tracking_number TEXT,
  carrier TEXT,
  status TEXT NOT NULL,
  last_event TEXT,
  estimated_delivery_at TEXT,
  delivered_at TEXT,
  updated_at TEXT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_shipments_tracking ON shipments(tracking_number);
CREATE INDEX IF NOT EXISTS idx_shipments_status ON shipments(status);
CREATE TABLE IF NOT EXISTS tasks(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  kind TEXT NOT NULL,
  payload_json TEXT NOT NULL,
  status TEXT NOT NULL DEFAULT 'pending',
  priority INTEGER NOT NULL DEFAULT 100,
  attempts INTEGER NOT NULL DEFAULT 0,
  max_attempts INTEGER NOT NULL DEFAULT 8,
  run_after TEXT NOT NULL,
  locked_by TEXT,
  locked_at TEXT,
  last_error TEXT,
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
CREATE INDEX IF NOT EXISTS idx_tasks_claim ON tasks(status, run_after, priority, id);
CREATE TABLE IF NOT EXISTS audit_log(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  level TEXT NOT NULL,
  category TEXT NOT NULL,
  message TEXT NOT NULL,
  correlation_id TEXT,
  context_json TEXT NOT NULL DEFAULT '{}',
  previous_hash TEXT,
  entry_hash TEXT,
  created_at TEXT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_audit_created ON audit_log(created_at);
CREATE TABLE IF NOT EXISTS metrics(
  name TEXT PRIMARY KEY,
  value REAL NOT NULL,
  labels_json TEXT NOT NULL DEFAULT '{}',
  updated_at TEXT NOT NULL
);
