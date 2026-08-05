#pragma once
#include "elit21/aliexpress/auth/AliExpressToken.h"
#include <chrono>
#include <mutex>
#include <optional>
namespace elit21::aliexpress::auth {class AliExpressTokenManager{public:void store(AliExpressToken token,std::chrono::system_clock::time_point issued_at=std::chrono::system_clock::now());[[nodiscard]]std::optional<AliExpressToken>get()const;[[nodiscard]]bool needsRefresh(std::chrono::minutes lead=std::chrono::minutes(15))const;private:mutable std::mutex mutex_;std::optional<AliExpressToken>token_;std::chrono::system_clock::time_point issued_at_{};};}
