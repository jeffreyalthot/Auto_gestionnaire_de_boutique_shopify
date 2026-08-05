#pragma once
#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>
namespace elit21::aliexpress::auth {class AliExpressOAuthStateStore{public:std::string issue(std::chrono::minutes ttl=std::chrono::minutes(10));bool consume(const std::string&state);private:std::mutex mutex_;std::unordered_map<std::string,std::chrono::steady_clock::time_point>states_;};}
