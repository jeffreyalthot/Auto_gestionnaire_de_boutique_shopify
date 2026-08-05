#pragma once
#include <algorithm>
#include <chrono>
namespace elit21::aliexpress::auth {class AliExpressTokenRefreshPolicy{public:static std::chrono::seconds retryDelay(int attempt){attempt=std::max(0,std::min(attempt,8));return std::chrono::seconds(1<<attempt);}};}
