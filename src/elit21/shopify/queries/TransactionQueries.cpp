#include "elit21/shopify/queries/TransactionQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument TransactionQueries::document(){return graphql::GraphqlDocument("OrderTransactions",R"graphql(query OrderTransactions($id:ID!){order(id:$id){transactions{id kind status amountSet{shopMoney{amount currencyCode}} gateway}}})graphql");}}
