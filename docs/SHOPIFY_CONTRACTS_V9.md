# Contrats Shopify v9

## Version ciblée

```text
2026-07
```

## Inventaire

- Mutation : `inventorySetQuantities`.
- Directive : `@idempotent(key: $idempotencyKey)`.
- Champ de comparaison : `changeFromQuantity`.
- Amorçage non comparé : `changeFromQuantity: null`, uniquement lorsque la politique l’autorise.
- Taille maximale d’un lot interne : 250 entrées.
- Champ interdit par l’audit : `ignoreCompareQuantity`.

## Catalogue externe

- Mutation : `productSet`.
- Identifiant métier : custom ID `elit21/aliexpress_product_id`.
- Variantes envoyées comme état complet du produit source.
- Mode asynchrone surveillé par `productOperation`.
- Chaque synchronisation est enregistrée dans `shopify_catalog_sync_runs`.

## GraphQL

- Analyse du coût et du statut de throttling.
- Reprises bornées avec jitter.
- `Retry-After` respecté.
- Circuit breaker pour les défauts réseau et HTTP 5xx.
- Les erreurs GraphQL et `userErrors` bloquent les transitions métier.

## Webhooks

- HMAC, domaine, version, empreinte, ordre temporel et idempotence.
- Mesure du taux d’échec, p90, reprises et abonnements retirés.
- Réconciliation lorsque l’ordre ou l’état devient incertain.

## Audit

L’audit de contrat est exécutable par :

```text
elit21_shopify_contract_auditor <racine> [config] [rapport]
```

Un score inférieur à 100 ou une violation bloquante doit empêcher l’activation du mode réel.
