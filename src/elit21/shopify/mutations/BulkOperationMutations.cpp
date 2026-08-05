#include "elit21/shopify/mutations/BulkOperationMutations.h"
namespace elit21::shopify::mutations {graphql::GraphqlDocument BulkOperationMutations::document(){return graphql::GraphqlDocument("BulkOperationRunQuery",R"graphql(mutation BulkOperationRunQuery($query:String!){bulkOperationRunQuery(query:$query){bulkOperation{id status} userErrors{field message}}})graphql");}}
