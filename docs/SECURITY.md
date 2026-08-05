# Sécurité

Les secrets sont chargés depuis les variables d'environnement. Les journaux ne doivent jamais recevoir les jetons. Le webhook Shopify exige un HMAC valide. Le mode réel est refusé lorsque les jetons manquent. Utilisez TLS sur le proxy public et limitez l'accès local au port webhook.
