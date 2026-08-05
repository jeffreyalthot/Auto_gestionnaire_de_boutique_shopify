#include "elit21/reports/ReportGenerator.h"
#include "elit21/util/TimeUtil.h"
#include <filesystem>
#include <fstream>
namespace elit21 {Result<void> ReportGenerator::write(const std::string&d,const RuntimeCounters&c)const{std::filesystem::create_directories(d);std::string p=d+"/report-"+std::to_string(util::unixMillis())+".json";std::ofstream f(p);if(!f)return Result<void>::failure("Impossible de créer "+p);Analytics a;auto j=a.snapshot(c);j.set("generated_at",util::utcNowIso());f<<j.dump(true)<<'\n';return Result<void>::success();}}
