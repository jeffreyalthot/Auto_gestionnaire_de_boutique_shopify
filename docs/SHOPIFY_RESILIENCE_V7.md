# Shopify Resilience v7

La v7 ajoute cinq garanties opérationnelles :

1. **Version exacte** : refus du fall-forward silencieux dans GraphQL.
2. **Idempotence durable** : réservation SQLite de 24 heures pour les mutations sensibles.
3. **Ordre des webhooks** : normalisation UTC, Event-Id, Webhook-Id et empreinte du payload.
4. **Inventaire CAS** : `changeFromQuantity`, relire puis retenter une fois en cas de conflit.
5. **Réconciliation suivie** : état et erreur conservés jusqu’à l’achèvement réel.

Les tables ajoutées par `0036_shopify_resilience_v7.sql` conservent les clés idempotentes, l’ordre par ressource, les demandes de réconciliation et les observations de version API.
