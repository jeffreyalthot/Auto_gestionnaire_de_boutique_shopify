# Description technique du fonctionnement du code source — version 6.0.0

## 1. Objectif et périmètre

ELIT21 Shopify × AliExpress Autonomous Manager est un gestionnaire terminal C++20 qui relie une boutique Shopify à AliExpress en tant que fournisseur pour le marché canadien. Le programme couvre le catalogue, les variantes, les prix CAD, les stocks, les commandes, les webhooks, les commandes fournisseur, le suivi, les remboursements, la confidentialité et la reprise après panne.

La version 6 conserve le mode protégé par défaut :

```text
dry_run=true
live_orders=false
verify_tls=true
country=CA
currency=CAD
markup_percent_before_shipping=100
```

La présence des chemins d’intégration ne signifie pas que des appels commerciaux réels ont été exécutés. Le mode réel dépend des comptes, jetons, permissions et moyens de paiement des plateformes.

## 2. Organisation du runtime

Le code est séparé en trois catégories :

1. `include/elit21/**` contient les interfaces, structures et contrats C++.
2. `src/elit21/**` contient les implémentations correspondantes.
3. `apps/**` et `tools/**` contiennent les exécutables terminaux qui assemblent les services.

`CMakeLists.txt` construit d’abord la bibliothèque statique `elit21_core`. Les applications et outils sont ensuite liés à cette bibliothèque. Les sources sont découvertes par `GLOB_RECURSE`, et chaque en-tête métier sous `include/elit21` possède un fichier `.cpp` correspondant sous `src/elit21`.

## 3. Séquence de démarrage

Le point d’entrée principal est `apps/manager/main.cpp`. Il instancie `elit21::Application`, charge `config/app.json`, initialise les signaux d’arrêt, puis démarre le runtime.

Le démarrage exécute les étapes suivantes :

1. lecture et validation de la configuration;
2. contrôle des invariants Canada/CAD et de la marge minimale;
3. ouverture de SQLite;
4. application des migrations versionnées;
5. initialisation de libcurl et TLS;
6. création des clients Shopify et AliExpress;
7. création du moteur d’autonomie;
8. activation du serveur webhook, du planificateur et des workers;
9. rendu du tableau terminal fixe;
10. démarrage des cycles périodiques.

Une erreur critique laisse le système en mode arrêté ou dégradé. Le programme ne bascule pas automatiquement en commandes réelles.

## 4. Configuration et secrets

`elit21::Config` agrège les sections application, réseau, base de données, Shopify, AliExpress, prix, stock, risque, automatisation et terminal.

Les secrets peuvent être fournis par configuration locale, variables d’environnement ou coffre chiffré. `ShopifyTokenManager` stocke les jetons avec AES-256-GCM, écrit les fichiers par remplacement atomique et ne journalise qu’une empreinte ou une représentation masquée.

`ShopifyProductionReadiness` contrôle notamment :

- le domaine Shopify;
- la version API;
- le jeton et le secret webhook;
- l’URL HTTPS publique;
- la vérification TLS;
- la devise CAD et le pays CA;
- la marge avant livraison;
- l’état de la base;
- la cohérence `dry_run`/`live_orders`;
- les identifiants AliExpress nécessaires au mode fournisseur réel.

## 5. Installation et OAuth Shopify

### 5.1 Génération de la configuration d’application

`ShopifyAppConfigGenerator` produit un fichier `shopify.app.toml` atomiquement. Il refuse les URL non HTTPS, les versions invalides et les listes de scopes vides. Le fichier généré contient :

- `client_id`;
- `application_url`;
- URLs de redirection OAuth;
- scopes;
- version des webhooks;
- sujets opérationnels;
- sujets de confidentialité.

L’outil terminal associé est :

```text
elit21_tool_shopify_app_config_generator
```

### 5.2 Création de l’URL d’autorisation

`ShopifyAuthorizationUrlBuilder` valide le magasin, le client ID, les scopes, le callback HTTPS et un état aléatoire d’au moins 32 caractères. Il encode les paramètres et construit l’URL d’autorisation.

### 5.3 État OAuth à usage unique

`ShopifyOAuthStateStore` génère un état cryptographiquement aléatoire, l’associe au magasin et au callback, puis lui applique une durée de vie. `consumeDetails()` supprime l’état dès sa consommation. Un deuxième callback utilisant le même état échoue.

### 5.4 Validation du callback

`ShopifyOAuthCallbackVerifier` :

1. limite la taille de la chaîne de requête;
2. décode strictement les paramètres;
3. refuse les paramètres dupliqués;
4. valide le domaine `*.myshopify.com`;
5. reconstruit le message canonique trié;
6. calcule HMAC-SHA256 en hexadécimal;
7. compare le HMAC en temps constant;
8. vérifie l’horodatage;
9. consomme l’état OAuth;
10. vérifie que l’état appartient au même magasin.

Le code d’autorisation n’est échangé contre un jeton qu’après ces validations.

### 5.5 Échange et conservation du jeton

`ShopifyOAuthClient` appelle `/admin/oauth/access_token`, analyse la réponse JSON, vérifie la longueur du jeton et conserve les scopes. `ShopifyTokenRefreshPolicy` classe les réponses en : aucune action, nouvelle tentative, vérification des scopes ou réautorisation.

## 6. Transport Shopify GraphQL

L’URL Admin GraphQL est produite par `ShopifyEndpoints` à partir du magasin et de la version API. Le client n’accepte pas un nom de magasin invalide et force HTTPS.

`ShopifyGraphqlClient` et `GraphqlTransport` :

- sérialisent l’opération et les variables en JSON;
- injectent `X-Shopify-Access-Token`;
- imposent TLS et les limites réseau;
- analysent les erreurs HTTP et GraphQL;
- lisent les extensions de coût;
- appliquent des reprises bornées;
- utilisent `Retry-After` lorsqu’il est présent;
- classent les erreurs retryables;
- enregistrent les métriques.

`ShopifyCostThrottler` maintient le budget de coût, la capacité maximale, les points disponibles et le taux de restauration. Avant une requête coûteuse, il calcule une attente recommandée.

`ShopifyGraphqlPaginator` conserve le curseur, refuse un curseur qui ne progresse pas et borne le nombre de pages.

## 7. Documents GraphQL, requêtes et mutations

Les dossiers `shopify/queries` et `shopify/mutations` contiennent des documents GraphQL versionnés pour :

- magasin;
- produits et variantes;
- collections et médias;
- inventaire et emplacements;
- clients et commandes;
- remboursements et retours;
- fulfillment orders et fulfillments;
- remises, marchés, fichiers et metafields;
- publications;
- webhooks;
- opérations Bulk.

`ShopifyQueryBuilder` valide les GID Shopify, filtre les champs demandés, normalise les chaînes et construit les filtres temporels sans accepter de caractères de contrôle.

`ShopifyUserErrorParser` parcourt les erreurs imbriquées et supprime les doublons. `GraphqlError` conserve le code, le message, le chemin, le request ID, la capacité de reprise et le délai recommandé.

## 8. Modèles Shopify typés

Le dossier `shopify/models` contient des modèles pour le magasin, les produits, variantes, médias, collections, inventaires, emplacements, clients, commandes, transactions, remboursements, retours, fulfillments, suivi, marchés, metafields, publications, webhooks et opérations Bulk.

Chaque modèle expose une combinaison de :

- champs typés;
- validation;
- conversion depuis JSON;
- conversion vers JSON;
- normalisation des identifiants et valeurs.

Les objets réseau ne sont pas insérés directement dans la base. Ils sont convertis vers les modèles locaux du domaine et validés avant persistance.

## 9. Catalogue et variantes

`ShopifyCatalogService`, `ShopifyProductService`, `ShopifyVariantService` et les mutations associées exécutent le parcours suivant :

1. création ou récupération du produit principal;
2. création des options;
3. création groupée des variantes;
4. association des SKU Shopify et AliExpress;
5. écriture du coût fournisseur dans les données locales;
6. création des metafields `elit21`;
7. mise à jour des prix;
8. mise à jour des niveaux de stock;
9. publication vers les canaux configurés.

Le stockage local sépare les produits et les variantes. La disponibilité, le coût, le prix et le stock sont suivis au niveau SKU.

## 10. Règle de prix

Tous les calculs critiques utilisent `ShopifyMoney`, qui stocke les montants en centimes signés plutôt qu’en virgule flottante.

L’invariant est :

```text
prix_avant_livraison = coût_fournisseur × 2
prix_final = prix_avant_livraison + livraison
```

Exemple :

```text
10,00 CAD × 2 + 3,00 CAD = 23,00 CAD
```

`ShopifyFeeEstimator` calcule séparément les frais configurables. Ils servent à l’analyse de rentabilité et ne modifient pas silencieusement l’invariant de majoration demandé.

## 11. Réception des webhooks Shopify

`WebhookHttpServer` reçoit les requêtes HTTPS transmises par l’infrastructure. `ShopifyWebhookProcessor` applique :

1. limite de taille;
2. contrôle du type de contenu;
3. validation du domaine du magasin;
4. vérification HMAC-SHA256 en temps constant;
5. lecture de la version API;
6. calcul SHA-256 du payload;
7. déduplication par identifiant webhook;
8. détection d’un même identifiant avec un corps différent;
9. écriture dans l’inbox SQLite;
10. création d’une tâche métier idempotente;
11. réponse HTTP rapide.

Les événements sont marqués traités seulement après réussite du worker. Un doublon valide reçoit une réponse de succès sans créer une deuxième opération.

## 12. Routage des sujets webhook

`ShopifyWebhookTopic` convertit le nom du sujet en enum et en type de tâche. Les sujets pris en charge comprennent :

```text
orders/paid
orders/cancelled
refunds/create
products/update
products/delete
inventory_levels/update
fulfillments/create
fulfillments/update
app/uninstalled
customers/data_request
customers/redact
shop/redact
```

Les handlers spécialisés normalisent la charge puis délèguent à la file de tâches.

## 13. Admission transactionnelle d’une commande

`ShopifyOrderAdmissionService` constitue la barrière principale avant toute commande fournisseur.

Pour chaque commande payée, il vérifie :

- devise CAD;
- destination canadienne;
- adresse et code postal;
- limite de risque;
- présence des lignes;
- quantité positive;
- mappage de chaque SKU AliExpress;
- coût et livraison actuels;
- stock supérieur au tampon de sécurité;
- prix payé suffisant pour couvrir coût × 2 + livraison;
- clé d’idempotence.

La commande, ses lignes, son événement initial et sa tâche sont écrits dans une transaction SQLite. Une anomalie ne disparaît pas : elle est persistée avec l’état `manual_review`.

## 14. Machine d’état des commandes

`ShopifyOrderStateMachine` interdit les transitions incohérentes. Les états couvrent notamment :

```text
received
validated
pending_supplier
supplier_ordered
shipped
delivered
manual_review
cancelled
refunded
failed
```

`Database::transitionOrderStatus` utilise une condition sur l’état courant. Deux workers ne peuvent donc pas appliquer simultanément une transition incompatible.

## 15. Prévalidation avant achat AliExpress

L’admission initiale n’est pas la dernière vérification. Juste avant l’achat, `OrderManager` relit :

- le mappage SKU;
- le coût fournisseur;
- le coût de livraison;
- le stock;
- le tampon de sécurité;
- la rentabilité minimale.

Une variation défavorable place la commande en `manual_review`. L’achat n’est pas exécuté avec des données périmées.

## 16. Intégration AliExpress

`AliExpressClient` et les services spécialisés utilisent les transports IOP/TOP configurés. Les requêtes comprennent les paramètres communs, la méthode, l’horodatage, le jeton et la signature.

Le système couvre notamment :

- détail produit;
- recommandations;
- recherche par image;
- calcul du transport vers le Canada;
- placement d’une commande;
- consultation de commande;
- suivi logistique;
- rapport de données Dropshipping.

Les appels fournisseur restent bloqués lorsque `live_orders=false`, lorsque les identifiants manquent ou lorsqu’une admission n’a pas réussi.

## 17. Worker, tâches et reprise

Les tâches sont stockées dans SQLite. Le worker :

1. réclame atomiquement une tâche disponible;
2. inscrit un verrou et une échéance;
3. exécute le type métier;
4. marque la tâche terminée;
5. planifie une reprise exponentielle en cas d’erreur retryable;
6. déplace l’échec définitif dans la dead-letter queue.

Les verrous expirés sont récupérables. Les tâches possèdent une clé de déduplication afin qu’un webhook répété ou un planificateur concurrent ne crée pas une deuxième action active.

Des leases nommés protègent aussi les cycles globaux comme la synchronisation Shopify.

## 18. Opérations Bulk

Les classes du dossier `shopify/bulk` prennent en charge :

- création d’une Bulk Query;
- surveillance avec délai maximal;
- annulation;
- récupération du statut;
- téléchargement atomique du JSONL;
- hash SHA-256;
- analyse JSONL en flux;
- staged uploads;
- importation ou exportation de produits, commandes et inventaire.

Le parseur traite une ligne à la fois afin de limiter la mémoire sur les machines modestes.

## 19. Fulfillment et suivi

Après création de la commande AliExpress, le worker interroge son état et son suivi. Lorsque le numéro de suivi existe :

1. normalisation du transporteur;
2. association aux fulfillment orders Shopify;
3. création ou mise à jour du fulfillment;
4. conservation du suivi local;
5. émission d’un événement d’audit;
6. mise à jour de l’état de la commande.

Les expéditions partielles conservent l’association par ligne et quantité.

## 20. Remboursements et retours

`ShopifyRefundExecutor` produit un plan validé : total payé, déjà remboursé, nouveau montant, solde remboursable et raison. Il bloque un montant nul, négatif ou supérieur au solde.

La classe ne dissimule pas la frontière d’exécution : un plan n’appelle pas automatiquement Shopify tant que le service de remboursement réel et le mode live ne sont pas explicitement activés.

## 21. Confidentialité Shopify

`ShopifyPrivacyService` gère :

- `customers/data_request` : export restreint et atomique;
- `customers/redact` : effacement des données du client et des commandes indiquées;
- `shop/redact` : effacement des données de la boutique.

Chaque demande est enregistrée, horodatée, auditée et marquée terminée. Les exports ne contiennent que les ressources demandées.

## 22. Base de données et migrations

SQLite utilise WAL, transactions et statements préparés. Les migrations sont numérotées et leur checksum est conservé. Un fichier déjà appliqué avec un contenu différent est détecté.

Les tables couvrent notamment :

- produits et variantes;
- stocks et historique;
- clients et commandes;
- lignes de commande;
- commandes fournisseur;
- fulfillments et suivi;
- remboursements et retours;
- webhooks inbox;
- outbox;
- tâches et dead letters;
- checkpoints;
- leases;
- événements d’état;
- demandes de confidentialité;
- audit;
- métriques et incidents;
- grand livre financier.

`backup_manager` utilise `sqlite3_backup`, puis exécute `PRAGMA integrity_check` et calcule SHA-256.

## 23. Sécurité

Les contrôles principaux sont :

- TLS obligatoire en production;
- HMAC constant pour webhooks et OAuth;
- AES-256-GCM pour les secrets locaux;
- états OAuth à usage unique;
- limites de taille réseau;
- délais de connexion et de requête;
- masquage des jetons, secrets, Bearer et courriels;
- audit chaîné SHA-256;
- statements SQLite préparés;
- validation stricte JSON et des identifiants;
- mode live explicitement séparé du dry run.

## 24. Terminal et observabilité

`Dashboard` et `FixedPositionRenderer` mettent à jour les mêmes positions au lieu d’imprimer continuellement de nouvelles lignes. `ShopifyPanel` expose :

- connexion;
- dry run/live;
- version API;
- score de préparation;
- requêtes GraphQL;
- reprises et throttles;
- webhooks acceptés et rejetés;
- tâches en attente;
- commandes en révision manuelle;
- dernier événement.

Les métriques sont également persistables et exportables en JSON.

## 25. Analyse de progression

`ProjectProgressAnalyzer` parcourt chaque fichier, associe les en-têtes à leur `.cpp`, classe les tests, détecte les implémentations Shopify, calcule les moyennes pondérées et génère :

- TREE CMD avec emojis;
- CSV par fichier;
- JSON global;
- moyennes par dossier;
- progression Shopify;
- profondeur des tests.

Les pourcentages indiquent la profondeur locale détectable. Ils ne constituent pas une certification des plateformes externes.

## 26. Tests

Les deux exécutables CTest couvrent notamment :

- monnaie en centimes;
- majoration de 100 %;
- estimation de frais;
- remboursement;
- machine d’état;
- HMAC OAuth et rejeu;
- génération `shopify.app.toml`;
- HMAC webhook et déduplication;
- versions, scopes et endpoints;
- coût GraphQL;
- pagination;
- modèles et services;
- migrations;
- admission transactionnelle;
- révision manuelle;
- prévalidation stock/prix;
- confidentialité;
- sauvegarde et reprise;
- AliExpress TOP;
- conformité canadienne.

## 27. Activation contrôlée du mode réel

Avant `live_orders=true`, le parcours recommandé est :

1. compiler et exécuter CTest;
2. générer et déployer `shopify.app.toml`;
3. installer l’application et terminer OAuth;
4. vérifier les scopes reçus;
5. publier le serveur webhook HTTPS;
6. déclencher des webhooks de test;
7. valider les migrations et sauvegardes;
8. connecter le compte AliExpress Dropshipping approuvé;
9. exécuter des commandes en dry run;
10. valider le paiement fournisseur et les règles de risque;
11. activer le mode réel avec une limite de commande basse;
12. surveiller la dead-letter queue, les audits et la réconciliation.

## 28. Limites techniques honnêtes

La compilation et les scénarios hors connexion valident le code local. Ils ne prouvent pas :

- l’approbation d’une application Shopify publique;
- l’obtention des scopes protégés;
- l’accès réel aux méthodes AliExpress Dropshipping;
- l’acceptation d’un paiement fournisseur;
- les délais et stocks réels d’un vendeur;
- la conformité juridique complète d’un produit donné;
- l’absence de changement futur dans les API.

Le programme conserve donc les limites externes visibles plutôt que de les simuler comme réussies.
