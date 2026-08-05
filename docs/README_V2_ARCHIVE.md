# ELIT21 Shopify × AliExpress Autonomous Manager 2.0.0

Complete C++20 terminal manager for a Shopify store using AliExpress as supplier for Canadian customers.

## Locked pricing invariant

```text
price_before_shipping = supplier_cost × 2
final_price = price_before_shipping + shipping
```

This applies a **100% markup before shipping**. Shipping is added only after the markup. Existing implementation and tests were preserved.

## Extended architecture

- Shopify Admin GraphQL, OAuth, webhooks, bulk operations and fulfillment orders.
- AliExpress IOP/TOP transports, OAuth, product discovery, freight, order and tracking services.
- Durable workflow, scheduler, inbox/outbox, dead-letter queue, idempotency and crash recovery.
- Product sourcing, catalog publication, stock synchronization, repricing, fulfillment, returns and finance.
- Canadian address and product screening, bilingual customer-service templates and manual review queues.
- Fixed-position terminal dashboard with accepted/rejected counters and bounded worker threads.
- 31 SQLite migrations, multiple service executables, operational tools and expanded test suites.

## Safety defaults

`dry_run=true` and `live_orders=false`. Real supplier orders remain blocked until credentials, API permissions, HMAC verification, idempotency storage and the live-order gate are explicitly enabled.

## Build

```bash
cmake -S . -B build/linux -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/linux --parallel 2
ctest --test-dir build/linux --output-on-failure
```

## Previous README content

# ELIT21 Shopify × AliExpress Autonomous Manager

Gestionnaire terminal C++ destiné à synchroniser une boutique Shopify avec AliExpress comme fournisseur pour des clients au Canada.

## Règle de prix obligatoire

Le programme applique exactement :

```text
marge = coût_fournisseur × 100%
prix_avant_livraison = coût_fournisseur + marge
prix_final = prix_avant_livraison + livraison
```

Exemple : produit 10 CAD, livraison 3 CAD → prix avant livraison 20 CAD → prix final 23 CAD.

Cette règle correspond à une majoration de 100% sur le coût fournisseur. Après la majoration, la livraison est ajoutée sans diminuer la majoration. Les frais Shopify, taxes, remboursements, variation de change et droits de douane peuvent réduire le bénéfice net réel.

## Fonctionnalités

- Shopify GraphQL Admin version configurable, valeur initiale `2026-07`.
- AliExpress Dropshipping via passerelle TOP avec signature MD5.
- Recherche, détails produit, transport Canada, commande fournisseur, état et suivi.
- Webhook Shopify `orders/paid` avec vérification HMAC SHA-256.
- SQLite avec migrations, idempotence et journal d'audit.
- Sourcing, restrictions produit, contrôle du code postal canadien et seuils de risque.
- Tableau terminal fixe avec compteurs acceptés/rejetés distincts.
- Mode autonome cyclique et rapports JSON.
- Mode `dry_run` activé par défaut.

## Dépendances

CMake 3.21+, compilateur C++20, libcurl, OpenSSL, SQLite3 et json-c.

### MSYS2 MinGW64

```bash
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-curl mingw-w64-x86_64-openssl mingw-w64-x86_64-sqlite3 mingw-w64-x86_64-json-c
cmake --preset msys2-mingw64-release
cmake --build --preset msys2-mingw64-release
ctest --preset msys2-mingw64-release
./build/msys2-mingw64-release/elit21_shop_manager.exe config/app.json
```

### Linux

```bash
sudo apt install build-essential cmake ninja-build libcurl4-openssl-dev libssl-dev libsqlite3-dev libjson-c-dev
cmake --preset linux-release
cmake --build --preset linux-release
ctest --preset linux-release
./build/linux-release/elit21_shop_manager config/app.json
```

## Mise en service

1. Copier `config/app.example.json` vers `config/app.json`.
2. Définir les secrets par variables d'environnement, jamais dans Git.
3. Joindre le Dropshipping Center AliExpress et faire approuver l'application.
4. Tester en `dry_run=true`.
5. Configurer le webhook public vers `/webhooks/shopify/orders-paid`.
6. Passer à `ELIT21_LIVE_ORDERS=true` seulement après tests et autorisations.

## Limites externes

Le programme ne contourne aucun CAPTCHA, validation de paiement, restriction de compte, autorisation protégée, contrôle de fraude, litige ou action humaine imposée par Shopify/AliExpress. L'API AliExpress Dropshipping dépend des permissions effectivement accordées au compte.

