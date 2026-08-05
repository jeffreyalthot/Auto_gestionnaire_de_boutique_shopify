#include "elit21/automation/AutomationEngine.h"

#include <chrono>
#include <thread>

namespace elit21 {
void AutomationEngine::run() {
    dashboard_.setStatus(config_.app.dry_run ? "ACTIF - DRY RUN" : "ACTIF - COMMANDES REELLES");

    while (!stop_) {
        const bool ali_ready = !config_.aliexpress.app_key.empty() &&
                               !config_.aliexpress.app_secret.empty();
        const bool shopify_ready = !config_.shopify.shop.empty() &&
                                   !config_.shopify.access_token.empty();

        if (config_.automation.catalog_sync && ali_ready) {
            dashboard_.setActivity("Recherche et evaluation des produits AliExpress", 0.10);
            catalog_.synchronize();
        } else if (config_.automation.catalog_sync) {
            dashboard_.setActivity("Catalogue en attente des cles AliExpress", 0.10);
        }

        if (config_.automation.inventory_sync && ali_ready) {
            dashboard_.setActivity("Synchronisation des stocks", 0.35);
            inventory_.synchronize();
        }

        if (config_.automation.order_processing && shopify_ready) {
            dashboard_.setActivity("Lecture des commandes Shopify payees", 0.55);
            orders_.pollShopify();
        }

        if (config_.automation.order_processing) {
            dashboard_.setActivity("Traitement des commandes fournisseur", 0.70);
            orders_.processPending();
        }

        if (config_.automation.tracking_sync && ali_ready) {
            dashboard_.setActivity("Synchronisation des suivis et expeditions", 0.85);
            fulfillment_.synchronize();
        }

        if (config_.automation.reports) {
            dashboard_.setActivity("Generation du rapport", 0.95);
            reports_.write("data/reports", counters_);
        }

        if (!ali_ready || !shopify_ready) {
            dashboard_.setStatus(config_.app.dry_run
                ? "DRY RUN - CONFIGURATION API INCOMPLETE"
                : "CONFIGURATION API INCOMPLETE");
        }
        dashboard_.setActivity("Cycle termine - attente", 1.0);

        for (int i = 0; i < config_.app.poll_seconds * 10 && !stop_; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    dashboard_.setStatus("ARRETE");
}
}
