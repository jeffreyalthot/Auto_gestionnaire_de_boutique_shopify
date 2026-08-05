#pragma once
#include "elit21/core/Result.h"
#include "elit21/core/Types.h"
#include <string>
#include <vector>
namespace elit21 {
class CanadaCompliance {public: explicit CanadaCompliance(std::vector<std::string> prohibited); static std::vector<std::string> loadKeywords(const std::string& path); bool validPostalCode(const std::string& value)const; Result<void> validateAddress(const Address& address,bool block_po_boxes)const; Result<void> validateProduct(const std::string& title,const std::string& description)const;private:std::vector<std::string> prohibited_;};
}
