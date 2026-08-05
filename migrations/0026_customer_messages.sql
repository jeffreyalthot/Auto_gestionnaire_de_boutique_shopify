CREATE TABLE IF NOT EXISTS customer_messages(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  order_id INTEGER REFERENCES orders(id) ON DELETE SET NULL,
  customer_id INTEGER REFERENCES customers(id) ON DELETE SET NULL,
  channel TEXT NOT NULL,
  template_key TEXT,
  locale TEXT NOT NULL DEFAULT 'fr-CA',
  subject TEXT,
  body_redacted TEXT NOT NULL,
  status TEXT NOT NULL,
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  sent_at TEXT
);
