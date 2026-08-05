#include "elit21/config/Config.h"
#include "elit21/storage/Database.h"
#include <filesystem>
#include <iostream>
int main(int argc,char**argv){const std::string cp=argc>1?argv[1]:"config/app.json";const std::string md_arg=argc>2?argv[2]:"";auto c=elit21::Config::load(cp);if(!c){std::cerr<<c.error()<<'\n';return 2;}if(const auto p=std::filesystem::path(c.value().app.database).parent_path();!p.empty())std::filesystem::create_directories(p);elit21::Database db;auto o=db.open(c.value().app.database);if(!o){std::cerr<<o.error()<<'\n';return 3;}const std::string md=md_arg.empty()?c.value().migrations_dir:md_arg;auto m=db.migrateDirectory(md);if(!m){std::cerr<<m.error()<<'\n';return 4;}std::cout<<"MIGRATIONS APPLIED: "<<md<<'\n';return 0;}
