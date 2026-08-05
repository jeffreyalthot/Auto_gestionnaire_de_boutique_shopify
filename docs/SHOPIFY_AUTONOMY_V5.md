# Shopify Autonomy v5

La version 5 ajoute un moteur d’autonomie Shopify dédié, distinct du worker général.

## Responsabilités

- validation stricte du domaine et de la version API;
- contrôle des scopes OAuth;
- stockage chiffré du jeton;
- test de santé GraphQL;
- lecture paginée des commandes payées non traitées;
- persistance idempotente dans SQLite;
- création dédupliquée des tâches fournisseur;
- réconciliation des webhooks opérationnels et de confidentialité;
- collecte des métriques GraphQL et de throttling;
- audit JSON de chaque cycle.

## Sécurité

Le mode réel exige HTTPS, un secret webhook et un jeton valide. Le mode `dry_run`
conserve l’ensemble du pipeline mais interdit les effets fournisseur irréversibles.

## Prix

La règle reste: `prix_avant_livraison = cout_fournisseur * 2`, puis la livraison est ajoutée.
