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
