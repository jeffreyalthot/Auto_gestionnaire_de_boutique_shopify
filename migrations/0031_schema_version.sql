CREATE TABLE IF NOT EXISTS service_leases(
  lease_name TEXT PRIMARY KEY,
  owner_id TEXT NOT NULL,
  acquired_at TEXT NOT NULL,
  expires_at TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS feature_flags(
  flag_name TEXT PRIMARY KEY,
  enabled INTEGER NOT NULL DEFAULT 0 CHECK(enabled IN (0,1)),
  configuration_json TEXT NOT NULL DEFAULT '{}',
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
INSERT OR IGNORE INTO feature_flags(flag_name, enabled, configuration_json)
VALUES('live_orders', 0, '{"requires_explicit_activation":true}');
