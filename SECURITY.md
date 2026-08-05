# Security Model

## Secrets

Never commit API tokens, application secrets, webhook secrets or encryption keys. Supply them through environment variables or an external secret store. Local encrypted values use AES-256-GCM with an authentication tag and associated data.

## Network

TLS certificate and host verification are enabled by default. Production mode rejects disabled TLS validation. HTTP clients enforce connection timeout, request timeout, response-size limits, redirect limits and low-speed aborts.

## Shopify webhooks

The receiver verifies `X-Shopify-Hmac-Sha256` in constant time, limits request size, records the payload hash, rejects a reused webhook ID with a different body, and stores the accepted event before returning success.

## Durable execution

Tasks use transaction-protected claims, worker leases, stale-lock recovery, bounded retries, active-task deduplication and a dead-letter table after retry exhaustion. Supplier orders use local idempotency keys and remain blocked unless live mode and credentials are both enabled.

## Logs and audit

Tokens, passwords, bearer headers and email addresses are redacted before writing logs. Security-sensitive events are also written to a SHA-256 chained audit log.

## Safe defaults

```text
dry_run=true
live_orders=false
verify_tls=true
country=CA
currency=CAD
markup_percent_before_shipping>=100
```

A security control reduces risk but does not replace account permissions, platform review, payment controls, legal review or infrastructure hardening.

## Shopify OAuth callback

The OAuth callback verifier canonicalizes parameters, removes signature fields, checks HMAC-SHA256 in constant time, validates timestamp freshness, validates the `myshopify.com` domain and consumes a single-use state tied to the expected shop. Duplicate query keys and malformed percent escapes are rejected.

## Transactional order boundary

Paid orders are persisted with their lines and initial event in one database transaction. Supplier purchase is preceded by a second stock/cost/mapping check. An invalid or stale order moves to manual review rather than being silently discarded or purchased.

## Privacy requests

Mandatory Shopify privacy requests are persisted before processing. Exports are written atomically, customer/shop redaction is scoped to the requested records, and completion state is recorded for audit.
