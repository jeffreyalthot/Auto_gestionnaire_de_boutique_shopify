#pragma once
#include <string>
#include <vector>
namespace elit21::aliexpress::models {struct AliExpressProduct{std::string product_id,title,description,seller_id,seller_name;double min_price_cad{0},max_price_cad{0},rating{0},seller_score{0};int orders{0},stock{0};std::vector<std::string> images;};}
