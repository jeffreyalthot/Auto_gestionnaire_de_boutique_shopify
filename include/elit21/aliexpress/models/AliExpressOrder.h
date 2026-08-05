#pragma once
#include <string>
#include <vector>
namespace elit21::aliexpress::models {struct AliExpressOrder{std::string order_id,status;double amount_cad{0};std::vector<std::string> line_ids;};}
