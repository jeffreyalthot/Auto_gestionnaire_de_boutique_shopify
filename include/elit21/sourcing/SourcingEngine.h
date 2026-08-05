#pragma once
#include "elit21/aliexpress/AliExpressClient.h"
#include "elit21/compliance/CanadaCompliance.h"
#include "elit21/config/Config.h"
#include "elit21/core/Result.h"
#include "elit21/risk/RiskEngine.h"
namespace elit21 {struct SourcingDecision{bool accepted{false};std::string reason;double score{0};};class SourcingEngine{public:SourcingEngine(SourcingConfig c,CanadaCompliance&co,RiskEngine&r):config_(c),compliance_(co),risk_(r){}SourcingDecision evaluate(const AliProduct&p,int delivery_days)const;private:SourcingConfig config_;CanadaCompliance&compliance_;RiskEngine&risk_;};}
