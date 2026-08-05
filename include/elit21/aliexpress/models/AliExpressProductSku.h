#pragma once
#include <string>
#include <map>
namespace elit21::aliexpress::models {struct AliExpressProductSku{std::string sku_id,sku_code;double price_cad{0};int stock{0};std::map<std::string,std::string> attributes;};}
