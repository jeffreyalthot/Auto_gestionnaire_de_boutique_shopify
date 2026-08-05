# Shopify Transactional v6

La version 6 ajoute une admission de commande atomique, une machine d’état persistante, une deuxième validation du coût et du stock avant achat, un traitement complet des webhooks de confidentialité, une validation HMAC du callback OAuth et un générateur `shopify.app.toml`.

## Invariants

- Monnaie critique stockée en centimes.
- Prix avant livraison égal au minimum à deux fois le coût fournisseur.
- Livraison ajoutée après la majoration.
- Aucune commande fournisseur sans mappage SKU, stock suffisant et mode live explicite.
- Aucune transition de commande hors machine d’état.
- Aucun callback OAuth sans HMAC, timestamp et état valide.
- Aucune demande de confidentialité perdue : réception, traitement et résultat sont persistés.

Consultez `TECHNICAL_CODE_OPERATION_V6.md` pour le parcours complet du runtime.
