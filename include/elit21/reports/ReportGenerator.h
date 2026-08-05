#pragma once
#include "elit21/analytics/Analytics.h"
#include "elit21/core/Result.h"
#include "elit21/core/Types.h"
#include <string>
namespace elit21 {class ReportGenerator{public:Result<void> write(const std::string&directory,const RuntimeCounters&c)const;};}
