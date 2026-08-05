#include "elit21/config/Config.h"
#include "elit21/storage/Database.h"
#include <iostream>
int main(int argc,char**argv){auto c=elit21::Config::load(argc>1?argv[1]:"config/app.json");if(!c){std::cerr<<c.error()<<'\n';return 2;}elit21::Database db;if(auto r=db.open(c.value().app.database);!r){std::cerr<<r.error()<<'\n';return 3;}if(auto r=db.migrateDirectory(c.value().migrations_dir);!r){std::cerr<<r.error()<<'\n';return 4;}auto reset=db.execute("UPDATE tasks SET status='pending',locked_by=NULL,locked_at=NULL,updated_at=CURRENT_TIMESTAMP WHERE status='processing';");if(!reset){std::cerr<<reset.error()<<'\n';return 5;}db.audit("WARNING","recovery","Processing tasks reset after emergency recovery");std::cout<<"RECOVERY COMPLETE\n";return 0;}
