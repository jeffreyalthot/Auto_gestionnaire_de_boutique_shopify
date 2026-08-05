CREATE TABLE IF NOT EXISTS product_mappings(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  aliexpress_product_id TEXT NOT NULL,
  aliexpress_sku_id TEXT NOT NULL,
  shopify_product_id TEXT NOT NULL,
  shopify_variant_id TEXT NOT NULL,
  shopify_inventory_item_id TEXT,
  mapping_status TEXT NOT NULL DEFAULT 'active',
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  UNIQUE(aliexpress_product_id, aliexpress_sku_id),
  UNIQUE(shopify_variant_id)
);
