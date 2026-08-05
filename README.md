# ELIT21 Shopify × AliExpress Autonomous Manager v9.0.0

**Édition Shopify Contracted — contrats Admin GraphQL 2026-07, catalogue `productSet`, inventaire idempotent et fiabilité webhook mesurable.**

Gestionnaire terminal C++20 pour automatiser une boutique Shopify alimentée par AliExpress sur le marché canadien. La v9 conserve tous les fichiers de la v8 et approfondit les chemins qui doivent rester compatibles avec le contrat Shopify réellement configuré.

## Progression

Les métriques définitives sont produites par `elit21_tool_project_progress_analyzer` dans :

```text
PROJECT_TREE_CMD_EMOJI_V9.txt
PROJECT_FILE_STATUS_V9.csv
PROJECT_STATUS_V9.json
PROJECT_DEEP_ANALYSIS_V9.md
```

## Invariant commercial

```text
prix_avant_livraison = coût_fournisseur × 2
prix_final = prix_avant_livraison + livraison
```

Chaque variante importée doit respecter une majoration minimale de 100 % avant livraison. Les calculs financiers critiques sont effectués en centimes.

## Nouveautés v9

- Contrat d’inventaire Shopify 2026-07 utilisant `changeFromQuantity`.
- Directive GraphQL `@idempotent(key: $idempotencyKey)` pour les écritures d’inventaire.
- Lots d’inventaire limités à 250 entrées.
- Synchronisation complète par `productSet` avec variantes, options, coûts et metafields AliExpress.
- Surveillance asynchrone des `ProductSetOperation`.
- Circuit breaker GraphQL fermé/ouvert/semi-ouvert.
- Audit de contrat Shopify empêchant le retour de champs retirés.
- Analyse SLO des webhooks : échecs, p90, reprises et suppression d’abonnement.
- Panneau terminal enrichi et rapports SQLite persistants.
- Applications `shopify_contract_auditor`, `shopify_catalog_sync` et `shopify_webhook_slo`.
- Résolution automatique des chemins runtime depuis le fichier de configuration.
- Taille des lots Unity configurable pour adapter le build aux machines à mémoire limitée.
- Bannières terminal synchronisées avec la version CMake `9.0.0`.

## Exécution sécurisée

```text
dry_run=true
live_orders=false
verify_tls=true
country=CA
currency=CAD
markup_percent_before_shipping=100
shopify_api_version=2026-07
```

Les commandes réelles exigent une activation explicite, des secrets valides et les permissions Shopify/AliExpress appropriées.

## Architecture opérationnelle

```text
Shopify HTTPS webhook
        │
        ├── domaine + HMAC + version + ordre temporel
        ├── idempotence et inbox SQLite
        └── tâche durable
                │
                ▼
Worker transactionnel
        │
        ├── admission de commande
        ├── validation SKU / stock / prix / risque
        ├── machine d’état
        ├── prévalidation fournisseur
        └── commande AliExpress dry-run ou live
                │
                ▼
Suivi et fulfillment Shopify
```

## Applications principales

```text
elit21_shop_manager                  Gestionnaire autonome et tableau terminal
elit21_autonomous_manager            Orchestration autonome avec garde-fous
elit21_shopify_control_center        Santé, synchronisation et opérations Shopify
elit21_shopify_contract_auditor      Audit bloquant du contrat Admin GraphQL 2026-07
elit21_shopify_catalog_sync          Planification/exécution productSet
elit21_shopify_webhook_slo           Mesure de fiabilité des livraisons webhook
elit21_shopify_governance            Audit de gouvernance hors connexion ou réel
elit21_shopify_readiness             Contrôle de préparation production
elit21_shopify_reconciler            Réconciliation persistante des écarts
elit21_webhook_server                Récepteur HMAC et mise en file idempotente
elit21_scheduler_service             Planification dédupliquée
elit21_worker_service                Exécution, reprise et dead-letter queue
elit21_database_migrator             Migrations versionnées avec checksums
elit21_backup_manager                Sauvegarde SQLite atomique et intégrité
elit21_replay_tool                   Rejeu de webhooks signé et idempotent
elit21_emergency_recovery            Récupération des tâches abandonnées
elit21_sandbox_simulator             Simulation financière hors connexion
```

## Outils principaux

```text
elit21_tool_project_progress_analyzer
elit21_tool_shopify_app_config_generator
elit21_tool_graphql_query_tester
elit21_tool_webhook_replayer
elit21_tool_database_inspector
elit21_tool_inventory_reconciler
elit21_tool_order_reconciler
elit21_tool_aliexpress_request_tester
elit21_tool_secret_generator
elit21_tool_log_analyzer
elit21_tool_product_mapping_editor
elit21_tool_shopify_product_set_planner
```

## Modes sûrs par défaut

```text
dry_run=true
live_orders=false
verify_tls=true
country=CA
currency=CAD
markup_percent_before_shipping=100
shopify_api_version=2026-07
```

Le mode réel exige les clés et jetons Shopify/AliExpress, les scopes, une URL HTTPS publique, l’inscription AliExpress Dropshipping, une méthode de paiement fournisseur et des essais sur une boutique contrôlée.

## Compilation Windows — MSYS2 MinGW64

```bash
pacman -S --needed \
  mingw-w64-x86_64-toolchain \
  mingw-w64-x86_64-cmake \
  mingw-w64-x86_64-ninja \
  mingw-w64-x86_64-curl \
  mingw-w64-x86_64-openssl \
  mingw-w64-x86_64-sqlite3 \
  mingw-w64-x86_64-json-c

cmake --preset msys2-mingw64-release
cmake --build --preset msys2-mingw64-release --parallel 4
ctest --preset msys2-mingw64-release
```

## Compilation Linux

```bash
sudo apt install build-essential cmake ninja-build \
  libcurl4-openssl-dev libssl-dev libsqlite3-dev libjson-c-dev

cmake --preset linux-release
cmake --build --preset linux-release --parallel 4
ctest --preset linux-release
```

## Démarrage recommandé

```bash
elit21_configuration_validator config/app.json
elit21_database_migrator config/app.json migrations
elit21_shopify_governance config/app.json
elit21_shopify_readiness config/app.json
elit21_shopify_control_center config/app.json validate
elit21_shop_manager config/app.json
```

L’option `--live` de l’application de gouvernance déclenche les contrôles réseau Shopify et ne doit être utilisée qu’avec des identifiants valides.

## Documentation

La description complète du code est disponible dans :

```text
docs/TECHNICAL_CODE_OPERATION_V8.md
docs/SHOPIFY_GOVERNANCE_V8.md
```

Les rapports et historiques des versions antérieures sont conservés dans l’archive.

## Exécution depuis n’importe quel dossier

Les chemins relatifs de `config/app.json` sont résolus depuis la racine du projet. Cette commande fonctionne donc même lorsque CMD ou PowerShell n’est pas positionné dans le dossier source :

```text
elit21_shop_manager C:\ELIT21\config\app.json
```

Le programme retrouve automatiquement `migrations/`, `data/` et `logs/`. `ELIT21_PROJECT_ROOT` peut remplacer cette racine lorsque la configuration est stockée ailleurs.

## Build adapté à 4 Go de RAM

```text
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DELIT21_UNITY_BATCH_SIZE=16
cmake --build build --parallel 4
ctest --test-dir build --output-on-failure -j 4
```
