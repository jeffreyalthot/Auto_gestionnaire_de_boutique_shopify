# Fonctionnement technique du code source — v9.0.0

## 1. Objectif du runtime

Le projet constitue un gestionnaire C++20 multi-service reliant une boutique Shopify à un fournisseur AliExpress. Il reçoit les événements Shopify, conserve les décisions dans SQLite, planifie les traitements, relit les conditions fournisseur, applique la règle de prix, place les commandes autorisées et renvoie les informations de suivi vers Shopify.

La v9 ajoute une couche de **contrats API** : une opération n’est pas considérée correcte seulement parce qu’elle compile; elle doit aussi respecter les champs, limites et comportements attendus de la version Shopify configurée.

## 2. Démarrage

```text
apps/manager/main.cpp
        │
        ├── Config::load(config/app.json)
        ├── validation CA / CAD / TLS / dry-run
        ├── Database::open()
        ├── Database::migrateDirectory(migrations/)
        ├── initialisation HttpClient
        ├── ShopifyClient
        ├── AliExpressClient
        ├── scheduler
        ├── workers
        ├── serveur webhook
        └── Dashboard terminal fixe
```

Les migrations sont appliquées dans l’ordre numérique et leurs checksums sont conservés. Une migration déjà appliquée avec un contenu différent déclenche une erreur plutôt qu’une modification silencieuse du schéma.

## 3. Transport Shopify GraphQL

`GraphqlTransport` construit la requête HTTP vers :

```text
https://<shop>.myshopify.com/admin/api/2026-07/graphql.json
```

Il ajoute le jeton Admin, vérifie TLS, impose les délais réseau, analyse le JSON GraphQL et collecte :

- coût demandé et coût réel;
- capacité et points disponibles;
- taux de restauration;
- `Retry-After`;
- identifiant de requête Shopify;
- version API servie;
- avertissements de dépréciation;
- erreurs réseau, HTTP, GraphQL et `userErrors`.

### Circuit breaker

`ShopifyGraphqlCircuitBreaker` protège Shopify et le worker lorsqu’un défaut réseau ou serveur se répète :

```text
closed --seuil d'échecs--> open --temporisation--> half_open
   ^                                             │
   └---------------- succès de la sonde --------┘
```

Une seule sonde est autorisée en état semi-ouvert. Les autres demandes sont rejetées localement. Une réponse 429 de limitation n’est pas classée comme panne serveur : elle utilise le mécanisme de throttling et de reprise.

## 4. Planification de l’inventaire

`ShopifyInventoryWritePlanner` reçoit des changements de stock internes et produit des lots GraphQL de 250 entrées maximum.

Pour une variante déjà connue :

```json
{
  "inventoryItemId": "gid://shopify/InventoryItem/...",
  "locationId": "gid://shopify/Location/...",
  "quantity": 9,
  "changeFromQuantity": 12
}
```

Pour un amorçage explicitement autorisé :

```json
{
  "inventoryItemId": "gid://shopify/InventoryItem/...",
  "locationId": "gid://shopify/Location/...",
  "quantity": 9,
  "changeFromQuantity": null
}
```

La mutation contient la directive idempotente et une clé différente pour chaque lot. Le planificateur rejette les GID invalides, quantités négatives, doublons et écritures non comparées non autorisées.

## 5. Synchronisation du catalogue par productSet

`ShopifyProductSetSyncService` transforme un produit AliExpress normalisé en entrée Shopify complète :

```text
Produit source
   ├── identifiant AliExpress
   ├── titre / description / vendeur
   ├── options
   ├── variantes
   │    ├── SKU ELIT21
   │    ├── SKU AliExpress
   │    ├── coût fournisseur
   │    ├── prix Shopify
   │    └── inventaire suivi
   └── metafields elit21
```

Le produit est identifié par le custom ID :

```text
namespace = elit21
key       = aliexpress_product_id
value     = <identifiant fournisseur>
```

Le service produit une clé métier déterministe, réserve l’opération dans le coordinateur d’idempotence, appelle `productSet`, enregistre le GID de l’opération asynchrone et peut interroger `productOperation` jusqu’à `COMPLETE`, `FAILED` ou expiration du délai.

## 6. Audit de contrat Shopify

`ShopifyApiContractAuditor` parcourt les sources Shopify et vérifie notamment :

- absence de `ignoreCompareQuantity`;
- présence de `changeFromQuantity`;
- mutation d’inventaire idempotente;
- présence du flux `productSet`;
- cohérence des versions API codées en dur;
- persistance des rapports.

L’application `elit21_shopify_contract_auditor` génère :

```text
SHOPIFY_API_CONTRACT_AUDIT_V9.md
score /100
nombre de fichiers analysés
avertissements
violations bloquantes
```

## 7. Réception des webhooks

Le serveur vérifie avant toute mise en file :

1. taille et méthode HTTP;
2. domaine `myshopify.com`;
3. signature HMAC en temps constant;
4. empreinte SHA-256 du corps;
5. identifiant d’événement et identifiant de livraison;
6. version API;
7. date `Triggered-At` normalisée en UTC;
8. doublon, rejeu ou événement périmé.

Un webhook récent crée une tâche durable. Un doublon retourne un succès sans retraitement. Un événement périmé déclenche une réconciliation au lieu d’écraser un état plus récent.

## 8. Mesure de fiabilité webhook

`ShopifyWebhookReliabilityMonitor` reçoit les échantillons de livraison et calcule :

- nombre total et réussites;
- taux d’échec;
- p90 du temps de réponse;
- plus grand nombre de reprises;
- abonnements retirés;
- état sain ou dégradé.

Le résultat est conservé dans `shopify_webhook_slo_audits` et affiché dans le panneau terminal Shopify.

## 9. Admission des commandes

Le webhook `orders/paid` déclenche `ShopifyOrderAdmissionService` :

```text
Commande payée
   ├── devise CAD
   ├── destination Canada
   ├── adresse / code postal
   ├── seuil de risque
   ├── mappage SKU Shopify ↔ AliExpress
   ├── stock fournisseur
   ├── coût fournisseur
   ├── transport
   └── marge minimale de 100 %
```

Une anomalie est persistée dans l’état `manual_review`. Une commande valide passe à `pending_supplier` avec sa tâche fournisseur dans la même transaction SQLite.

## 10. Prévalidation AliExpress

Juste avant l’achat, le worker relit le mappage, le coût, le transport et le stock. La commande n’est pas envoyée lorsque les données ont changé défavorablement. En mode dry-run, un identifiant `DRYRUN-...` est généré sans transaction commerciale.

## 11. Règle de prix

```text
prix_avant_livraison = coût_fournisseur × 2
prix_final = prix_avant_livraison + livraison
```

Les montants critiques sont représentés en centimes entiers. Exemple :

```text
coût       10,00 CAD
base       20,00 CAD
livraison   3,00 CAD
final      23,00 CAD
```

## 12. Worker, reprises et file morte

Chaque tâche possède une clé de déduplication, une priorité, une date de disponibilité, un lease de worker et un nombre maximal d’essais.

```text
pending → processing → completed
                  ├── retry avec backoff exponentiel
                  └── dead_letter après l'échec terminal
```

Les leases abandonnés sont récupérés au redémarrage. Le journal d’audit conserve l’action et l’erreur.

## 13. Fulfillment

Lorsqu’AliExpress fournit le transporteur et le suivi, le système normalise l’événement logistique, retrouve le `FulfillmentOrder` Shopify, crée ou met à jour le fulfillment et déclenche la notification client selon la configuration.

## 14. Stockage et sécurité

SQLite contient les produits, variantes, mappages, commandes, tâches, événements, webhooks, audits, métriques, idempotence, réconciliations et historiques. Les secrets locaux peuvent être chiffrés en AES-256-GCM; les journaux masquent jetons, mots de passe, en-têtes Bearer et données sensibles.

## 15. Tableau terminal

Le rendu fixe affiche sans faire défiler les lignes principales :

- mode dry-run/live;
- connexion Shopify/AliExpress;
- version API;
- score du contrat;
- état du circuit GraphQL;
- coût et throttling;
- webhooks acceptés/rejetés;
- taux d’échec et p90;
- tâches en attente;
- commandes en révision;
- dernier événement notable.

## 16. Limite de validation

La compilation, les migrations, les tests hors connexion, les plans GraphQL et les audits locaux sont validés. Une certification de production requiert une boutique Shopify contrôlée, un jeton Admin, les scopes, une URL HTTPS publique, un compte AliExpress Dropshipping autorisé et une méthode de paiement réelle.


## 17. Résolution des chemins runtime

`Config::load()` normalise le chemin du fichier JSON, détermine la racine du projet et convertit les chemins locaux en chemins absolus :

```text
config/app.json
      │
      ├── project_root = dossier parent de config/
      ├── data_dir     = <project_root>/data
      ├── log_file     = <project_root>/logs/elit21.log
      ├── database     = <project_root>/data/elit21.db
      └── migrations   = <project_root>/migrations
```

La variable `ELIT21_PROJECT_ROOT` ou le champ `application.project_root` permet d’utiliser une configuration placée hors du projet. Les applications et outils utilisent ensuite `config.migrations_dir`, ce qui élimine la dépendance au répertoire courant du terminal.

## 18. Construction sur machine à mémoire limitée

Le noyau active le Unity Build, mais la taille des lots est configurable :

```text
-DELIT21_UNITY_BATCH_SIZE=16
--parallel 4
```

La reconstruction Release propre v9 a terminé avec quatre tâches parallèles. Le pic résident observé lors de la dernière phase mesurée était de 330 940 KiB, sans swap. Le lot peut être réduit à 8 si un compilateur Windows consomme davantage de mémoire.

## 19. Validation depuis un autre dossier

Le binaire principal a été lancé depuis `/tmp` avec un chemin absolu vers `config/app.json`. Il a retrouvé les 38 migrations, ouvert la base sous la racine du projet et affiché le tableau terminal `9.0.0` sans erreur de chemin.
