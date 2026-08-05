# Fonctionnement technique du code source — ELIT21 v8.0.0 Shopify Governed

## 1. Finalité du système

ELIT21 est un gestionnaire terminal C++20 qui relie une boutique Shopify Admin GraphQL à un fournisseur AliExpress. SQLite constitue la source de vérité opérationnelle locale. Les événements externes sont convertis en messages persistants, puis exécutés par des workers séparés afin que la réception HTTP, la logique métier et les appels fournisseur ne dépendent pas du même processus.

Le projet est structuré autour de quatre invariants :

1. aucune commande fournisseur ne doit être créée deux fois;
2. aucun événement Shopify ancien ne doit écraser un état plus récent;
3. aucun stock ne doit être remplacé aveuglément lorsqu’une quantité distante est connue;
4. aucun produit ne doit être publié sous la majoration minimale demandée.

La règle de prix reste :

```text
prix_avant_livraison = coût_fournisseur × 2
prix_final = prix_avant_livraison + livraison
```

Les montants critiques sont ramenés en centimes avant comparaison.

## 2. Organisation du dépôt

```text
apps/                       exécutables de service
include/elit21/             interfaces publiques C++
src/elit21/                 implémentations correspondantes
migrations/                 évolution transactionnelle SQLite
tests/                      scénarios unitaires, intégration et panne
config/                     configurations sûres et exemples
resources/                  politiques, modèles et correspondances
tools/                      utilitaires terminaux
docs/                       architecture et procédures
cmake/                      options, dépendances et toolchains
packaging/                  installation Windows/Linux/portable
```

Chaque en-tête sous `include/elit21` possède un fichier `.cpp` correspondant sous `src/elit21`, y compris les composants déclaratifs conservés pour la conformité structurelle du TREE.

## 3. Séquence de démarrage

Les principaux points d’entrée sont `apps/manager/main.cpp`, `apps/autonomous_manager/main.cpp` et les services spécialisés.

```text
main
 │
 ├─ Config::load(config/app.json)
 ├─ Config::validate()
 ├─ Database::open()
 ├─ Database::migrateDirectory(migrations/)
 ├─ construction HttpClient
 ├─ construction ShopifyClient
 ├─ construction AliExpressClient
 ├─ démarrage scheduler / worker / webhook
 └─ rendu terminal et supervision
```

### Validation initiale

La configuration vérifie notamment :

- pays `CA`;
- devise `CAD`;
- majoration avant livraison supérieure ou égale à 100 %;
- cohérence entre `dry_run` et `live_orders`;
- TLS actif pour la production;
- domaine Shopify normalisable;
- version API au format trimestriel;
- URL webhook HTTPS;
- limites de file, reprises et délais réseau.

Une configuration réelle incomplète est refusée avant l’ouverture du cycle commercial.

## 4. Couche réseau

`HttpClient` encapsule libcurl et applique :

- délais de connexion et de réponse;
- vérification TLS;
- fichier CA optionnel;
- proxy optionnel;
- limites de taille de réponse;
- en-têtes structurés;
- codes HTTP et erreurs réseau normalisés;
- masquage des secrets avant journalisation.

`GraphqlTransport` ajoute les règles Shopify :

- URL `/admin/api/<version>/graphql.json`;
- jeton `X-Shopify-Access-Token`;
- nom et empreinte de l’opération;
- reprises bornées avec jitter;
- respect de `Retry-After`;
- détection de `THROTTLED`;
- suivi du coût GraphQL;
- capture de `X-Request-ID`;
- capture de l’avertissement de dépréciation;
- rejet d’une version réellement servie différente de la version configurée.

Les métriques restent accessibles au centre de contrôle terminal.

## 5. Gouvernance de version Shopify v8

### 5.1 Calendrier embarqué

`ShopifyApiVersionGuard` contient les fenêtres trimestrielles connues par le programme. Il sait :

- valider le format `YYYY-MM`;
- déterminer si une version est connue;
- calculer sa période de support;
- identifier la version stable configurée;
- détecter un fall-forward ou un décalage de version.

### 5.2 Catalogue public dynamique

`ShopifyApiVersionCatalog` complète le calendrier local par la requête GraphQL `publicApiVersions`.

```text
ShopifyClient
   │
   ├─ ApiVersionQueries::publicVersions()
   ├─ GraphqlTransport::execute()
   └─ ShopifyApiVersionCatalog::parse()
             │
             ├─ handle
             ├─ displayName
             ├─ supported
             └─ observation temporelle
```

Le catalogue peut être :

- découvert en direct;
- validé contre la version configurée;
- persisté dans SQLite;
- relu hors connexion;
- utilisé pour détecter qu’une version annoncée n’est plus supportée.

### 5.3 Audit de gouvernance

`ShopifyOperationalGovernance` exécute deux modes.

**Mode hors connexion** : configuration, sécurité, version, scopes, webhooks, base, idempotence, compare-and-set, confidentialité et catalogue local.

**Mode réel** : ajoute la santé Shopify, `publicApiVersions`, version servie, dépréciations et état des identifiants.

Chaque contrôle produit :

- un code stable;
- un niveau obligatoire ou informatif;
- une réussite ou un échec;
- un message;
- une preuve structurée.

Le rapport complet, le score et la décision sont enregistrés dans `shopify_governance_runs`. Les observations du catalogue sont enregistrées dans `shopify_api_catalog_observations`.

L’application `elit21_shopify_governance` exécute ces contrôles sans appel réseau par défaut; `--live` active les contrôles Shopify réels.

## 6. OAuth Shopify

Le chemin OAuth est séparé de l’exécution commerciale.

1. `ShopifyAuthorizationUrlBuilder` construit l’URL avec scopes et état.
2. `ShopifyOAuthStateStore` persiste un état à usage unique lié au magasin.
3. `ShopifyOAuthCallbackVerifier` valide domaine, timestamp, paramètres uniques et HMAC.
4. `ShopifyOAuthClient` échange le code contre un jeton.
5. `ShopifyTokenManager` chiffre le jeton avec AES-256-GCM.
6. La politique de réautorisation vérifie scopes et expiration logique.

Un état consommé ne peut pas être rejoué. Le callback n’accepte pas de paramètres dupliqués ni de domaine hors `myshopify.com`.

## 7. Mutations Shopify gouvernées

`ShopifyMutationCoordinator` constitue la barrière commune des mutations sensibles.

```text
Demande de mutation
      │
      ├─ validation du document GraphQL
      ├─ vérification qu’il s’agit d’une mutation
      ├─ normalisation des variables
      ├─ empreinte SHA-256
      ├─ réservation idempotente SQLite
      └─ exécution Shopify
              │
              ├─ erreurs transport/GraphQL → failed
              ├─ userErrors → failed
              └─ succès → completed + réponse persistée
```

Une clé déjà complétée avec les mêmes paramètres retourne la réponse enregistrée. La même clé associée à d’autres paramètres est rejetée. `ShopifyGovernedMutationService` expose ce mécanisme aux workers et aux services de haut niveau.

## 8. Modèles Shopify et validation commerciale

`ShopifyModels` définit les objets utilisés par le client : produits, variantes, prix, inventaire, fulfillment, webhooks et opérations Bulk.

La v8 ajoute `ShopifyModelValidator`.

### Produit

Le validateur contrôle :

- titre obligatoire et longueur maximale;
- vendeur;
- identifiant produit AliExpress;
- images uniquement en HTTPS;
- SKU Shopify uniques;
- identifiants SKU AliExpress uniques;
- option et valeur de variante;
- coût et prix finis et positifs;
- stock non négatif;
- concordance produit parent/variante;
- prix au moins égal au coût multiplié par `1 + marge/100`.

### Lots

Les lots de prix et de stock contrôlent :

- taille maximale;
- GID Shopify attendu;
- identifiants uniques;
- montants valides;
- quantité non négative;
- présence du `compare_quantity` lorsque le compare-and-set est exigé.

## 9. Registre des webhooks

`ShopifyWebhookRegistry` construit un plan avant toute modification distante.

Les abonnements sont classés :

```text
KEEP                 abonnement exact requis
CREATE               sujet requis absent
DELETE_DUPLICATE     doublon exact sous contrôle ELIT21
DELETE_OBSOLETE      callback ELIT21 non prévu par le plan courant
IGNORE_EXTERNAL      abonnement externe conservé
```

Le mode dry-run ne crée et ne supprime rien. Même en mode réel, la suppression ne s’exécute que lorsque `prune_managed=true`. Un callback extérieur à `webhook_base_url` est toujours préservé.

Les sujets requis couvrent commandes, remboursements, produits, inventaire, fulfillment, désinstallation et confidentialité.

## 10. Réception des webhooks

`ShopifyWebhookProcessor` réalise :

1. validation HTTP;
2. limite de taille;
3. validation du magasin;
4. comparaison HMAC en temps constant;
5. empreinte du corps;
6. validation de la version API;
7. lecture des identifiants d’événement et de livraison;
8. normalisation RFC 3339 vers UTC;
9. contrôle d’ordre par ressource;
10. inbox idempotente;
11. insertion d’une tâche durable;
12. réponse rapide au serveur Shopify.

Un doublon retourne une réponse réussie sans retraitement. Un événement périmé crée une demande de réconciliation au lieu de modifier directement l’état.

## 11. Idempotence et ordre temporel

### Idempotence

`shopify_idempotency_keys` enregistre :

- magasin;
- opération;
- clé;
- hash des paramètres;
- état;
- réponse;
- expiration;
- erreur.

### Ordre des webhooks

`shopify_webhook_ordering` conserve le dernier événement par magasin, type et ressource :

- instant UTC;
- Event-Id;
- Webhook-Id;
- hash du payload;
- compteurs accepté, dupliqué et périmé.

La comparaison se fait sur des instants normalisés plutôt que sur la chaîne RFC 3339 brute.

## 12. Catalogue AliExpress vers Shopify

`CatalogManager` orchestre :

1. découverte du produit AliExpress;
2. récupération des variantes;
3. filtrage Canada et produits interdits;
4. estimation du transport;
5. calcul du prix;
6. validation par `ShopifyModelValidator`;
7. création du produit Shopify;
8. création des variantes;
9. liaison SKU AliExpress/Shopify;
10. stock initial;
11. metafields de provenance;
12. publication sur le canal prévu;
13. checkpoint SQLite.

Un produit invalidant la marge ou contenant des SKU dupliqués n’est pas publié.

## 13. Synchronisation d’inventaire compare-and-set

`InventoryManager` travaille par SKU.

```text
Stock AliExpress
      │
      ├─ retrait du stock de sécurité
      ├─ lecture quantité Shopify mémorisée
      ├─ lecture InventoryLevel si inconnue
      └─ inventorySetQuantities
              ├─ quantity
              ├─ compare_quantity
              └─ clé idempotente
```

Si Shopify signale un conflit, le niveau est relu et une seule nouvelle tentative est créée avec une nouvelle réservation idempotente. La quantité confirmée est persistée pour le cycle suivant.

## 14. Admission et traitement des commandes

Le webhook `orders/paid` ou le poller construit une commande locale.

`ShopifyOrderAdmissionService` vérifie :

- devise CAD;
- destination Canada;
- adresse et code postal;
- score de risque;
- SKU et mappage fournisseur;
- coût et livraison;
- disponibilité;
- tampon de sécurité;
- prix payé;
- marge minimale.

La transaction SQLite enregistre atomiquement :

- commande;
- lignes;
- événement;
- décision;
- tâche fournisseur.

Une anomalie est conservée en `manual_review`. Elle n’est pas supprimée.

## 15. Machine d’état des commandes

`ShopifyOrderStateMachine` limite les transitions :

```text
received
  └─ validated
       ├─ pending_supplier
       │    └─ supplier_ordered
       │          └─ shipped
       │                └─ delivered
       └─ manual_review

États terminaux ou alternatifs : cancelled, refunded, failed
```

Les changements utilisent une condition sur l’état précédent afin de réduire les courses entre workers.

## 16. Prévalidation AliExpress

Avant l’achat réel, `OrderManager` relit :

- produit et SKU;
- coût actuel;
- livraison actuelle;
- stock actuel;
- tampon;
- total payé;
- marge;
- état local.

Une variation défavorable déplace la commande vers `manual_review`. En dry-run, un identifiant fournisseur simulé est produit sans transaction commerciale.

## 17. Client AliExpress

`AliExpressClient` et les couches IOP/TOP assurent :

- OAuth;
- signature TOP;
- requêtes produit et recommandation;
- recherche par image;
- calcul de fret vers le Canada;
- création de commande;
- lecture de commande;
- suivi logistique;
- normalisation des erreurs et statuts.

Les méthodes disponibles dépendent des permissions du compte AliExpress Dropshipping.

## 18. Tâches, workers et reprise

La table `tasks` fournit :

- type;
- payload JSON;
- priorité;
- exécution différée;
- clé de déduplication;
- worker propriétaire;
- lease;
- essais;
- erreur;
- file morte.

Le worker :

1. récupère les verrous abandonnés;
2. réclame un lot dans une transaction;
3. exécute le gestionnaire typé;
4. complète, reprogramme ou termine en échec;
5. déclenche une compensation ou réconciliation lorsque nécessaire.

## 19. Réconciliation

`ShopifyReconciliationCoordinator` suit :

```text
pending → running → completed
                  └→ failed → running
```

Les cycles couvrent :

- catalogue;
- inventaire;
- commandes;
- fulfillment;
- suivi;
- webhooks;
- versions API.

Le nombre de tentatives et la dernière erreur restent persistants.

## 20. Bulk Operations

La couche Bulk fournit :

- démarrage d’une query;
- surveillance bornée;
- annulation;
- inventaire des opérations;
- téléchargement HTTPS atomique;
- SHA-256;
- lecture JSONL en flux;
- export produits, commandes et inventaire;
- staged uploads;
- reprise sur données partielles.

Le parseur JSONL ne charge pas nécessairement la totalité du fichier en mémoire, ce qui convient au matériel limité.

## 21. Fulfillment et suivi

`FulfillmentManager` :

1. interroge AliExpress;
2. récupère numéro de suivi et transporteur;
3. normalise les événements;
4. récupère les `FulfillmentOrder` Shopify;
5. crée ou met à jour le fulfillment;
6. persiste l’expédition;
7. déclenche la notification client;
8. surveille retard, perte et livraison.

## 22. Retours, remboursements et confidentialité

Les remboursements sont planifiés en centimes et limités au solde remboursable. La mutation réelle reste séparée du calcul.

`ShopifyPrivacyService` traite :

```text
customers/data_request
customers/redact
shop/redact
```

Les demandes sont persistées avant traitement. Les exports sont atomiques et les suppressions sont auditables.

## 23. Sécurité

- AES-256-GCM pour le coffre local;
- HMAC-SHA256 pour webhooks et OAuth;
- comparaison en temps constant;
- SHA-256 pour payloads, audits et manifestes;
- génération aléatoire cryptographique;
- masquage des secrets et données personnelles dans les logs;
- TLS vérifié;
- scopes minimaux configurables;
- états OAuth à usage unique;
- aucune clé réelle dans l’archive.

## 24. Base de données et migrations

Les migrations sont exécutées dans l’ordre numérique et leur checksum est enregistré. Une migration déjà appliquée dont le contenu change est refusée.

Les domaines principaux sont :

- produits et variantes;
- inventaire et historique;
- commandes et lignes;
- commandes fournisseur;
- expéditions et suivi;
- tâches et dead-letter;
- webhooks et ordre temporel;
- idempotence;
- checkpoints;
- réconciliations;
- OAuth et installations;
- confidentialité;
- gouvernance et versions API;
- audit et finances.

SQLite est configuré avec contraintes et transactions. Les sauvegardes utilisent `sqlite3_backup`, puis `PRAGMA integrity_check` et SHA-256.

## 25. Terminal et observabilité

Le tableau fixe met à jour les mêmes positions au lieu d’imprimer continuellement de nouvelles lignes.

Il affiche :

- connexions Shopify/AliExpress;
- mode dry-run/live;
- version API configurée et servie;
- score de gouvernance;
- coût GraphQL;
- throttling et reprises;
- webhooks acceptés, dupliqués, périmés et rejetés;
- tâches en attente, traitement et échec;
- commandes en révision;
- stocks, prix et expéditions;
- dernier événement notable.

## 26. Applications et séparation des processus

Les exécutables peuvent être déployés séparément :

- manager;
- serveur webhook;
- worker;
- scheduler;
- gouvernance;
- readiness;
- réconciliation;
- OAuth;
- migration;
- sauvegarde;
- diagnostic;
- récupération d’urgence;
- simulation.

Cette séparation permet de redémarrer un worker sans interrompre l’entrée webhook et de mettre à l’échelle les traitements sans multiplier les récepteurs HTTP.

## 27. Tests

Les scénarios locaux couvrent notamment :

- règle de prix;
- Canada et codes postaux;
- AliExpress TOP;
- migrations et checksums;
- inbox webhook et rejeu;
- tâches et leases;
- OAuth chiffré;
- versions API;
- catalogue `publicApiVersions`;
- mutations idempotentes;
- modèles et lots Shopify;
- plan webhook et préservation des abonnements externes;
- GID 64 bits;
- timestamps et ordre des événements;
- inventaire compare-and-set;
- réconciliation;
- chiffrement et redaction.

Les tests réseau commerciaux réels ne sont pas inclus, car ils nécessitent des comptes et transactions contrôlés.

## 28. Génération du TREE

`ProjectProgressAnalyzer` parcourt chaque fichier, mesure sa profondeur détectable et produit :

- arbre CMD avec emojis;
- CSV par fichier;
- JSON global;
- pourcentages projet, sources, Shopify, tests et configuration.

La mesure n’est pas une certification. Elle distingue toutefois les modules dédiés, les adaptateurs partagés, les tests de scénario, la configuration et les fichiers purement documentaires.

## 29. Limites de production

La compilation et les scénarios hors connexion démontrent la cohérence locale. Une exploitation commerciale exige encore :

- application Shopify installée;
- jetons et scopes approuvés;
- endpoint HTTPS public;
- secret webhook;
- compte AliExpress Dropshipping accepté;
- méthode de paiement;
- vérification de produits et obligations réglementaires;
- essais contrôlés sur des commandes de faible valeur;
- supervision et procédures humaines pour litiges et exceptions.

La livraison conserve donc :

```text
dry_run=true
live_orders=false
verify_tls=true
country=CA
currency=CAD
markup_percent_before_shipping=100
```
