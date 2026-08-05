# Fonctionnement technique — ELIT21 v7.0.0 Shopify Resilient

## 1. Objectif du runtime

Le programme est un gestionnaire terminal C++20 qui relie une boutique Shopify à un fournisseur AliExpress. Il conserve SQLite comme source de vérité locale et utilise des tâches persistantes pour découpler les webhooks, les synchronisations et les commandes fournisseur.

La règle commerciale centrale reste :

```text
prix_avant_livraison = coût_fournisseur × 2
prix_final = prix_avant_livraison + livraison
```

Tous les montants transactionnels critiques sont validés en CAD et les protections `dry_run=true` et `live_orders=false` demeurent actives par défaut.

## 2. Démarrage

1. `apps/manager/main.cpp` ou `apps/autonomous_manager/main.cpp` charge `config/app.json`.
2. `Config` valide Shopify, AliExpress, TLS, Canada, CAD et la marge minimale.
3. `Database` ouvre SQLite, active les contraintes et applique les migrations dans l’ordre.
4. `HttpClient` applique délais, proxy, certificat CA, limite de réponse et vérification TLS.
5. Les clients `ShopifyClient` et `AliExpressClient` sont construits.
6. Le serveur webhook, le planificateur, les workers et le tableau terminal peuvent être exécutés séparément ou par le gestionnaire.

## 3. Entrée Shopify par webhook

`ShopifyWebhookProcessor` exécute la chaîne suivante :

1. vérification de la méthode, du chemin, du type MIME et de la taille;
2. validation du domaine `myshopify.com` attendu;
3. vérification HMAC en temps constant;
4. calcul SHA-256 du corps et enregistrement dans l’inbox `webhooks`;
5. validation de la version API annoncée;
6. lecture de `X-Shopify-Webhook-Id`, `X-Shopify-Event-Id` et `X-Shopify-Triggered-At`;
7. normalisation RFC 3339 vers UTC par `ShopifyTimestamp`;
8. détection du doublon et de l’événement périmé par ressource;
9. insertion d’une tâche durable dédupliquée;
10. réponse HTTP rapide, le traitement métier étant confié au worker.

Un webhook périmé n’écrase pas l’état local. Il crée une demande de réconciliation persistante. Un horodatage invalide est également isolé et déclenche une réconciliation plutôt qu’une mutation incertaine.

## 4. Ordonnancement et idempotence

### Webhooks

`shopify_webhook_ordering` mémorise, pour chaque boutique, type et identifiant de ressource :

- dernier instant UTC accepté;
- dernier Event-Id;
- dernier Webhook-Id;
- dernière empreinte du payload;
- compteurs acceptés, périmés et dupliqués.

Les valeurs absentes ne remplacent jamais un checkpoint déjà connu.

### Mutations Shopify

`ShopifyIdempotencyKey` produit des clés aléatoires ou déterministes. `ShopifyIdempotencyStore` réserve chaque opération dans `shopify_idempotency_keys` pendant 24 heures et interdit qu’une même clé soit réutilisée avec d’autres paramètres.

États :

```text
reserved → in_progress → completed
                     └→ failed → in_progress
```

Une opération complétée renvoie son résultat mis en cache au lieu d’être réémise.

## 5. GraphQL Admin

`GraphqlTransport` :

- impose HTTPS;
- ajoute le jeton Admin;
- détecte le nom de l’opération;
- applique des reprises bornées avec jitter;
- respecte `Retry-After`;
- suit le coût demandé, réel et disponible;
- détecte `THROTTLED` et les réponses 429/5xx;
- refuse une réponse servie sous une version API différente de la version demandée;
- expose des métriques thread-safe au terminal et aux diagnostics.

`ShopifyApiVersionGuard` connaît les dates de sortie et de fin de support des versions trimestrielles. La v7 utilise `2026-07` comme version stable configurée.

## 6. Identifiants Shopify

`ShopifyGlobalId` analyse et génère les identifiants :

```text
gid://shopify/<Type>/<Id>?paramètre=valeur
```

Les identifiants numériques sont conservés en `uint64_t`, ce qui évite toute perte de précision pour des valeurs supérieures à la limite exacte des nombres JavaScript.

## 7. Catalogue AliExpress vers Shopify

`CatalogManager` :

1. récupère les produits et variantes AliExpress;
2. applique les règles Canada et les exclusions;
3. calcule coût, livraison, marge et prix final;
4. construit le produit Shopify, ses options et variantes;
5. crée les metafields de liaison AliExpress;
6. lie chaque SKU à un `ProductVariant` et un `InventoryItem` Shopify;
7. persiste les mappages dans SQLite;
8. publie le produit sur la publication configurée.

## 8. Inventaire résilient

`InventoryManager` travaille au niveau du SKU.

1. Il relit le stock fournisseur et soustrait le tampon de sécurité.
2. Il calcule le prix actuel.
3. Il récupère la quantité Shopify mémorisée.
4. Pour une quantité connue, il émet `inventorySetQuantities` avec `changeFromQuantity`.
5. L’écriture porte une clé idempotente et un URI de document d’audit.
6. Pour une quantité inconnue, il interroge d’abord l’`InventoryLevel` Shopify.
7. Si le niveau existe, l’écriture passe en compare-and-set.
8. Seul un niveau réellement absent utilise l’amorçage contrôlé sans comparaison.
9. En cas de conflit, la quantité distante est relue, persistée et la mutation est retentée une fois avec une nouvelle réservation idempotente.
10. La quantité confirmée est stockée dans `variants.shopify_quantity`.

Cette stratégie empêche un cycle local périmé d’écraser une vente ou une correction récente effectuée sur Shopify.

## 9. Commandes

Le webhook `orders/paid` ou le poller construit un `CustomerOrder`. `ShopifyOrderAdmissionService` vérifie :

- CAD et destination canadienne;
- adresse et code postal;
- limite de risque;
- mappage de chaque SKU;
- coût fournisseur et livraison actuels;
- stock et tampon de sécurité;
- marge minimale de 100 % avant livraison.

La commande, ses lignes, l’événement et la tâche fournisseur sont enregistrés dans une transaction SQLite. Une anomalie est persistée sous `manual_review` au lieu d’être perdue.

Avant l’achat AliExpress, `OrderManager` relit les coûts, les stocks et les mappages. Une variation défavorable bloque l’achat et remet la commande en révision.

## 10. AliExpress

`AliExpressClient` signe les requêtes TOP/IOP, appelle les méthodes produit, transport, commande et suivi, puis normalise les réponses. En mode `dry_run`, un identifiant fournisseur déterministe est créé sans transaction commerciale réelle.

## 11. Tâches et reprise

La table `tasks` implémente :

- priorités;
- date d’exécution;
- verrou de worker;
- nombre maximal d’essais;
- reprise exponentielle;
- déduplication des tâches actives;
- récupération des verrous abandonnés;
- file morte après échec terminal.

Le worker prend une tâche dans une transaction, exécute le gestionnaire correspondant, puis la complète ou la reprogramme.

## 12. Réconciliation

`ShopifyReconciliationCoordinator` crée des réconciliations par ressource et des cycles périodiques pour :

- catalogue;
- inventaire;
- commandes;
- fulfillment et suivi;
- abonnements webhook.

Chaque demande possède une clé fondée sur la ressource et l’événement source. Son cycle est :

```text
pending → running → completed
                  └→ failed → running
```

Une réconciliation nécessitant des identifiants n’est jamais marquée terminée si ceux-ci sont absents.

## 13. Fulfillment

`FulfillmentManager` lit le statut AliExpress, normalise le transporteur et le numéro de suivi, récupère les `FulfillmentOrder` Shopify, crée le fulfillment et persiste l’expédition. Les cycles ultérieurs surveillent retard, perte et livraison.

## 14. Sécurité

- AES-256-GCM pour les secrets locaux;
- HMAC-SHA256 pour OAuth et webhooks;
- comparaison en temps constant;
- SHA-256 pour payloads, audits et manifestes;
- masquage des jetons, courriels et mots de passe dans les journaux;
- TLS obligatoire en production;
- états OAuth à usage unique;
- aucune clé privée réelle dans l’archive.

## 15. Confidentialité Shopify

`ShopifyPrivacyService` traite :

```text
customers/data_request
customers/redact
shop/redact
```

Les demandes sont persistées avant traitement, les exports sont écrits atomiquement et les suppressions sont auditables.

## 16. Observabilité

Le terminal fixe et les rapports affichent :

- état Shopify et AliExpress;
- version API demandée et servie;
- coût GraphQL disponible;
- reprises et throttling;
- webhooks acceptés/rejetés;
- tâches en attente et en erreur;
- commandes en révision;
- stocks, prix, commandes et bénéfice estimé.

## 17. Limite de validation

La compilation, SQLite, les scénarios hors connexion et le mode `dry_run` sont validés localement. Une certification commerciale en production exige les comptes, jetons, permissions, paiements et webhooks HTTPS réels.
