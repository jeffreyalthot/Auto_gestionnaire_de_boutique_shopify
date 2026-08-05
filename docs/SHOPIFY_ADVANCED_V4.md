# Shopify Advanced Runtime — version 4.0.0

## Objectif

La version 4 renforce le chemin Shopify du gestionnaire autonome sans retirer les fonctions AliExpress, Canada, prix, inventaire, commandes, fulfillment, sécurité et terminal déjà présentes.

## Fonctions implémentées

- Transport GraphQL Admin avec reprise bornée, temporisation exponentielle, lecture de `Retry-After`, détection de `THROTTLED` et métriques de coût.
- Création de produits puis création en lot de variantes Shopify, avec SKU, coût fournisseur, suivi d'inventaire, options et metafields de liaison AliExpress.
- Mise à jour en lots des prix de variantes et des quantités d'inventaire.
- Pagination des commandes payées non traitées avec curseur vérifié et checkpoint SQLite persistant.
- Création, inventaire et suppression des abonnements webhook requis.
- Récepteur webhook commun vérifiant HMAC, domaine de boutique, identifiant de webhook et rejeu.
- Conversion des webhooks en tâches idempotentes pour les commandes, remboursements, produits, stocks, fulfillments et désinstallation.
- Opérations Shopify Bulk: démarrage, inventaire, consultation et annulation.
- Analyseur de progression du projet produisant un TREE CMD avec emojis et pourcentage par fichier.

## Règle financière conservée

```text
prix_avant_livraison = cout_fournisseur_CAD * 2
prix_final = prix_avant_livraison + livraison_CAD
```

## Mode réel

Le projet est livré avec `dry_run=true` et `live_orders=false`. Le mode réel exige des identifiants Shopify/AliExpress valides, les scopes nécessaires, une URL HTTPS publique pour les webhooks et les permissions du programme AliExpress Dropshipping.
