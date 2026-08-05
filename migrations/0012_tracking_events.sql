CREATE TABLE IF NOT EXISTS tracking_events(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  shipment_id INTEGER NOT NULL REFERENCES shipments(id) ON DELETE CASCADE,
  event_code TEXT,
  status TEXT NOT NULL,
  description TEXT,
  location TEXT,
  happened_at TEXT,
  raw_json TEXT NOT NULL DEFAULT '{}',
  UNIQUE(shipment_id, status, happened_at, description)
);
