# Production Readiness

## Validated locally

- C++20 Release configuration and compilation.
- All project applications and operational tools link successfully.
- CTest suites pass.
- SQLite migrations apply in numeric order.
- Scheduler deduplicates active jobs.
- Worker executes and completes scheduled jobs.
- End-to-end local webhook scenario: valid HMAC accepted, duplicate recognized, invalid HMAC rejected, order queued, order persisted and supplier order simulated.
- Pricing invariant: supplier cost multiplied by two before shipping is added.
- AES-256-GCM encryption, tamper rejection and log redaction.

## Required before live orders

- Register and approve Shopify and AliExpress applications.
- Confirm API scopes and Dropshipping access.
- Store secrets outside the source tree.
- Deploy behind HTTPS and a reverse proxy with rate limiting.
- Configure backups and monitor dead letters, audit-chain verification and task age.
- Validate taxes, customs, returns, consumer disclosures and product restrictions for the actual catalog.
- Test payment capture and supplier ordering with low-value controlled orders.

## Intentional controls

The project fails closed for missing credentials, non-CAD orders, non-Canadian destinations, invalid postal codes, insufficient product mappings, invalid HMAC values and attempts to enable live ordering without explicit authorization.
