#pragma once
#include <map>
#include <string>
#include <vector>
namespace elit21::util {
std::string trim(std::string value);
std::string lower(std::string value);
std::string upper(std::string value);
std::string replaceAll(std::string value, const std::string& from, const std::string& to);
std::string urlEncode(const std::string& value);
std::string urlDecode(const std::string& value);
std::string join(const std::vector<std::string>& values, const std::string& separator);
bool containsInsensitive(const std::string& text, const std::string& needle);
std::string sanitizeTerminal(std::string value);
}
