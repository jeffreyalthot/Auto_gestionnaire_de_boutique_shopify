# Worker and Webhook Pipeline

```text
Shopify orders/paid
  → request-size limit
  → HMAC SHA-256 verification
  → webhook ID + body hash persistence
  → duplicate/replay decision
  → durable shopify_order_paid task
  → worker lease
  → strict JSON and monetary parsing
  → exact 64-bit Shopify IDs
  → Canadian address/risk validation
  → idempotent local order
  → place_supplier_order task
  → dry-run simulation or authorized AliExpress order
  → tracking synchronization
  → Shopify fulfillmentCreate
```

Failures are retried with bounded exponential delays. Exhausted tasks are copied to `dead_letters`. Worker leases older than the configured recovery threshold are returned to the pending queue.
