# Gouvernance Shopify v8

## Objectif

La gouvernance v8 empêche l’activation d’un chemin Shopify réel lorsqu’un prérequis technique n’est pas démontré. Elle ne remplace pas l’autorisation des plateformes; elle transforme les conditions de production en contrôles explicites, persistants et auditables.

## Contrôles hors connexion

- configuration Canada et CAD;
- marge minimale de 100 % avant livraison;
- cohérence `dry_run` / `live_orders`;
- domaine Shopify normalisé;
- endpoint GraphQL versionné;
- version API reconnue dans le calendrier embarqué;
- URL webhook HTTPS;
- scopes requis;
- sujets de confidentialité;
- persistance SQLite;
- idempotence des mutations;
- compare-and-set d’inventaire;
- catalogue API mis en cache, lorsqu’il existe.

## Contrôles réels

Le mode `--live` ajoute :

- requête de santé Shopify;
- découverte `publicApiVersions`;
- validation de la version configurée;
- comparaison avec la version réellement servie;
- collecte des avertissements de dépréciation;
- persistance de l’observation dans SQLite.

## Historique

Chaque exécution produit une ligne dans `shopify_governance_runs` avec :

- boutique;
- mode hors connexion ou réel;
- version API;
- score;
- décision de préparation;
- rapport JSON complet;
- horodatage.

Les observations de `publicApiVersions` sont stockées dans `shopify_api_catalog_observations`.

## Mutations gouvernées

`ShopifyMutationCoordinator` réserve une clé idempotente avant l’appel réseau. La clé est associée à l’empreinte des variables. Un rejeu identique retourne le résultat persistant; une réutilisation avec des paramètres différents est rejetée.

Les erreurs GraphQL et `userErrors` empêchent la mutation d’être marquée réussie. Le résultat terminal est auditée dans SQLite.

## Registre webhook

Le planificateur classe les abonnements :

- exacts et conservés;
- manquants et à créer;
- doublons gérés;
- obsolètes appartenant au domaine ELIT21;
- externes et préservés.

La suppression n’est exécutée que lorsque `prune_managed=true`, que le mode n’est pas dry-run et que l’URL appartient à la base webhook configurée.
