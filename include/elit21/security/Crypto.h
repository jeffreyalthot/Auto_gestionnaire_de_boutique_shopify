#pragma once
#include <map>
#include <string>
namespace elit21::crypto {
std::string md5Hex(const std::string& data,bool uppercase=false);
std::string sha256Hex(const std::string& data);
std::string hmacSha256Base64(const std::string& key,const std::string& data);
std::string hmacSha256Hex(const std::string& key,const std::string& data);
std::string base64Encode(const unsigned char* data,std::size_t size);
bool constantTimeEquals(const std::string& a,const std::string& b);
std::string randomHex(std::size_t bytes);
std::string aliExpressTopSign(const std::map<std::string,std::string>& parameters,const std::string& secret);
}
