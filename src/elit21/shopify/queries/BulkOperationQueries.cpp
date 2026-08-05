#include "elit21/shopify/queries/BulkOperationQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument BulkOperationQueries::document(){return graphql::GraphqlDocument("CurrentBulkOperation",R"graphql(query CurrentBulkOperation { currentBulkOperation { id status errorCode objectCount fileSize url partialDataUrl createdAt completedAt } })graphql");}}
