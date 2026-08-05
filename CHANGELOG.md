# Changelog

## 9.0.0 — Shopify Contracted

- Remplacement complet de l’ancien contrat d’inventaire par `inventorySetQuantities` 2026-07 avec directive `@idempotent` et champ `changeFromQuantity`.
- Suppression du chemin `ignoreCompareQuantity` et ajout d’un audit empêchant sa réintroduction.
- Ajout de `ShopifyInventoryWritePlanner` avec lots de 250 entrées, compare-and-set et amorçage explicite par `null`.
- Ajout de `ShopifyProductSetSyncService` pour la synchronisation idempotente d’un catalogue externe, variantes incluses, avec surveillance de `ProductSetOperation`.
- Ajout de l’application `elit21_shopify_catalog_sync` et de l’outil `elit21_tool_shopify_product_set_planner`.
- Ajout de `ShopifyGraphqlCircuitBreaker` avec états fermé, ouvert et semi-ouvert, métriques et protection des sondes concurrentes.
- Intégration du circuit breaker dans le transport GraphQL, sans ouverture sur les réponses de throttling 429.
- Ajout de `ShopifyApiContractAuditor`, score de conformité, rapport Markdown/JSON et persistance SQLite.
- Ajout de `ShopifyWebhookReliabilityMonitor` pour le taux d’échec, le p90, le budget de reprises et les abonnements retirés.
- Enrichissement du panneau terminal Shopify avec circuit GraphQL, score contractuel et métriques webhook.
- Ajout de la migration 0038 pour les audits contractuels, synchronisations catalogue et audits SLO webhook.
- Ajout d’exemples exécutables pour `productSet` et les métriques de livraison webhook.
- Renforcement des tests de contrat 2026-07, `productSet`, circuit breaker, SLO et persistance.
- Compilation Release complète, CTest 2/2 et audit contractuel 100/100.
- Résolution absolue de `data`, `logs`, `database` et `migrations` depuis la racine déclarée par la configuration.
- Démarrage validé depuis un répertoire externe à la racine du projet.
- Bannières des exécutables synchronisées avec `ELIT21_VERSION` au lieu d’une version codée en dur.
- Taille des unités Unity configurable par `ELIT21_UNITY_BATCH_SIZE`; validation finale avec lot 16 et parallélisme 4.

## 8.0.0 — Shopify Governed

- Ajout de `ShopifyApiVersionCatalog` pour découvrir, analyser, mettre en cache et valider `publicApiVersions`.
- Ajout de `ShopifyOperationalGovernance` avec contrôles hors connexion/réels, score, historique SQLite et observations de versions.
- Ajout de l’application `elit21_shopify_governance`.
- Ajout de `ShopifyMutationCoordinator` et `ShopifyGovernedMutationService` pour l’idempotence durable, le rejeu et l’audit des mutations.
- Capture des identifiants de requête Shopify et des avertissements de dépréciation GraphQL.
- Mise à jour du calendrier trimestriel des versions Shopify et détection stricte du fall-forward.
- Ajout de la migration 0037 pour les audits de gouvernance et les observations du catalogue API.
- Ajout d’une requête GraphQL dédiée à `publicApiVersions`.
- Générateur `shopify.app.toml` désormais paramétrable et validé.
- Validation métier approfondie des produits, variantes, images HTTPS, SKU, prix et majoration minimale de 100 %.
- Validation des lots de prix et d’inventaire avec unicité et compare-and-set.
- Registre webhook planifié : abonnements requis, manquants, doublons, obsolètes gérés et externes préservés.
- Renforcement des tests de gouvernance, modèles, idempotence et planification des webhooks.
- Documentation technique et TREE de progression v8.

## 7.0.0 — Shopify Resilient

- Ajout de l’idempotence Shopify persistante sur 24 heures.
- Validation stricte de la version API servie et détection du fall-forward.
- Analyse des GID Shopify sans perte des identifiants 64 bits.
- Normalisation RFC 3339 de `Triggered-At` vers UTC.
- Détection des webhooks dupliqués et périmés par ressource.
- Réconciliation durable avec états, tentatives et erreurs.
- Inventaire `inventorySetQuantities` idempotent avec compare-and-set.
- Lecture des InventoryLevel, amorçage contrôlé et reprise unique après conflit.
- Persistance de la quantité Shopify par variante/SKU.
- Application `shopify_reconciler` et scénarios de tests dédiés.
- Documentation technique complète v7.

## 6.0.0 — Shopify Transactional

- Ajout de `ShopifyMoney` pour tous les montants critiques en centimes.
- Ajout de `ShopifyOrderAdmissionService` avec persistance atomique, idempotence et révision manuelle.
- Ajout de `ShopifyOrderStateMachine` et transitions SQLite conditionnelles.
- Prévalidation finale du coût, stock, livraison, mappage et prix avant achat AliExpress.
- Conservation des commandes anormales au lieu de leur suppression.
- Ajout de `ShopifyPrivacyService` pour export, redaction client et redaction boutique.
- Ajout de `ShopifyOAuthCallbackVerifier` : HMAC-SHA256, timestamp, domaine et état à usage unique.
- Ajout de `ShopifyAppConfigGenerator` et de l’outil terminal générant `shopify.app.toml`.
- Ajout de `ShopifyProductionReadiness` et de l’application de contrôle associée.
- Ajout d’un planificateur de remboursement et d’un estimateur configurable des frais Shopify.
- Ajout d’un panneau terminal Shopify typé.
- Ajout de migrations pour leases, événements de commandes, installations et confidentialité.
- Renforcement des tests sur OAuth, rejeu, admission, état, confidentialité, prix, stocks et configuration d’application.
- Ajout d’une description technique détaillée du fonctionnement du code source.

## 5.0.0 — Shopify Autonomy

- Ajout de `ShopifyAutonomyEngine` avec santé, métriques, audit, checkpoints et ingestion idempotente.
- OAuth Shopify complet : URL d’autorisation, états à usage unique, scopes, jetons chiffrés et politique de réautorisation.
- Validation des versions trimestrielles, domaines `myshopify.com`, endpoints et permissions.
- 24 modèles Shopify typés avec validation et sérialisation JSON.
- 22 services Shopify dédiés pour catalogue, produits, variantes, médias, collections, inventaire, commandes, clients, remises, marchés, fichiers, metafields, retours, remboursements, fulfillment, publications, webhooks et Bulk.
- Cœur GraphQL enrichi : empreinte d’opération, variables déclarées, chemins d’erreur, erreurs utilisateur, pagination protégée et coût restauré dans le temps.
- Opérations Bulk réelles : démarrage, surveillance, annulation, JSONL en flux, téléchargement atomique, hash SHA-256 et staged uploads.
- Webhooks commerciaux et confidentialité, HMAC constant, rejeu, domaine, déduplication et files persistantes.
- Nouveau centre de contrôle Shopify terminal.
- Sauvegarde SQLite cohérente avec API `sqlite3_backup`, contrôle d’intégrité et empreinte SHA-256.
- Validateur de configuration renforcé avec garde-fous production.
- Outil de rejeu webhook signé et idempotent.
- Simulateur financier sans exception de conversion et majoration minimale de 100 %.
- Tests renforcés pour OAuth, erreurs, limites GraphQL, factory, modèles, Bulk, webhooks, SQLite et autonomie.
- TREE et mesure de progression v5 calculés par fichier.

## 4.0.0 — Shopify Advanced

- Transport GraphQL Shopify résilient avec coût, throttling, Retry-After et métriques.
- Produits Shopify à variantes multiples via opérations bulk de variantes.
- Mise à jour groupée des prix et de l’inventaire.
- Pagination des commandes avec checkpoints SQLite persistants.
- Registre et processeur webhook commun, HMAC, domaine, rejeu et tâches idempotentes.
- Gestion worker des annulations, remboursements, produits, stocks, fulfillments et désinstallation.
- Migration 0033 pour checkpoints, réconciliation et échantillons API.
- Analyseur C++ du taux de progression par fichier et TREE CMD avec emojis.
- Tests approfondis Shopify, webhook, pagination, coût GraphQL et base de données.

## 3.0.0

- Conserved every file and functional path from version 2.0.0.
- Replaced the former hash-only generic behavior with a domain-aware `BusinessComponent` engine.
- Added dedicated Shopify GraphQL, OAuth, webhook, product, variant, inventory, publication and fulfillment implementations.
- Added dedicated AliExpress request models, method catalog, OAuth, TOP signing, freight, order and tracking implementations.
- Rebuilt the SQLite layer with 32 versioned migrations, WAL, idempotent orders, SKU variants, active-task deduplication, stale-lock recovery, retry scheduling and dead-letter persistence.
- Added a real task dispatcher for catalog, inventory, orders, tracking, reports and Shopify paid-order webhooks.
- Hardened webhook processing with exact 64-bit IDs, robust decimal parsing, HMAC verification, payload-hash replay protection and retryable state transitions.
- Added AES-256-GCM authenticated encryption and systematic secret/PII redaction in logs.
- Replaced superficial hash tests with business, database, GraphQL, OAuth, webhook, security and resilience scenarios.
- Preserved the mandatory 100% markup before shipping and safe defaults `dry_run=true`, `live_orders=false`.

## 2.0.0

- Expanded the project to the complete target architecture supplied by the user.
- Preserved every existing source module and the 100% pre-shipping markup rule.
- Added managed implementations for all missing C++ headers and matching source files.
- Added Shopify OAuth/GraphQL/bulk/webhook services and AliExpress IOP/TOP/dropshipping services.
- Added durable workflows, scheduler, queues, recovery, observability, finance, Canadian compliance and terminal modules.
- Added 31 migrations, service applications, tools, packaging assets and extended tests.

## Previous history

# Changelog

## 1.0.0
- Complete C++ terminal manager skeleton with real HTTP, SQLite, Shopify GraphQL, AliExpress TOP signing, webhooks, autonomous workflows and tests.
- Enforced default pricing rule: supplier product cost + 100% markup, then shipping is added separately.
- Canada-first defaults: destination CA, currency CAD, bilingual notification templates.
