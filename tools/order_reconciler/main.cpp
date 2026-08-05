#include "elit21/config/Config.h"
#include "elit21/storage/Database.h"
#include <iostream>
int main(int argc,char**argv){auto c=elit21::Config::load(argc>1?argv[1]:"config/app.json");if(!c){std::cerr<<c.error()<<'\n';return 2;}elit21::Database db;if(auto r=db.open(c.value().app.database);!r){std::cerr<<r.error()<<'\n';return 3;}if(auto r=db.migrateDirectory(c.value().migrations_dir);!r){std::cerr<<r.error()<<'\n';return 4;}const char*states[]={"pending_supplier","supplier_ordered","shipped","delivered"};for(auto state:states){auto orders=db.ordersByStatus(state,10000);if(!orders){std::cerr<<orders.error()<<'\n';return 5;}std::cout<<state<<'='<<orders.value().size()<<'\n';}return 0;}
