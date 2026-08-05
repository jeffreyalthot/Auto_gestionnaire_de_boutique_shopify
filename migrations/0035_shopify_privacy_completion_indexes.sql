CREATE INDEX IF NOT EXISTS idx_shopify_privacy_requests_shop_topic
ON shopify_privacy_requests(shop_domain,topic,received_at);

CREATE INDEX IF NOT EXISTS idx_orders_customer_email
ON orders(customer_email) WHERE customer_email IS NOT NULL AND customer_email<>'';

CREATE INDEX IF NOT EXISTS idx_shopify_order_events_transition
ON shopify_order_events(from_status,to_status,created_at);
