#include "elit21/sourcing/SourcingEngine.h"
#include <algorithm>
namespace elit21 {SourcingDecision SourcingEngine::evaluate(const AliProduct&p,int d)const{auto c=compliance_.validateProduct(p.title,p.description);if(!c)return{false,c.error(),0};auto r=risk_.approveSupplier(p.seller_score,p.rating,p.orders,d,config_);if(!r)return{false,r.error(),0};double score=std::clamp(p.seller_score*.4+p.rating*10*.3+std::min(p.orders,1000)/1000.0*30,0.0,100.0);return{true,"Produit admissible au Canada",score};}}
