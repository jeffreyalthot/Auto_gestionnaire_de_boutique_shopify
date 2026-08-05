#include "elit21/aliexpress/auth/AliExpressOAuthStateStore.h"
#include "elit21/security/Crypto.h"
namespace elit21::aliexpress::auth {std::string AliExpressOAuthStateStore::issue(std::chrono::minutes ttl){auto s=crypto::randomHex(32);std::lock_guard l(mutex_);states_[s]=std::chrono::steady_clock::now()+ttl;return s;}bool AliExpressOAuthStateStore::consume(const std::string&s){std::lock_guard l(mutex_);auto i=states_.find(s);if(i==states_.end())return false;bool ok=i->second>=std::chrono::steady_clock::now();states_.erase(i);return ok;}}
