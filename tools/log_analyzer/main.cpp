#include <fstream>
#include <iostream>
#include <map>
#include <string>
int main(int argc,char**argv){const std::string path=argc>1?argv[1]:"logs/elit21.log";std::ifstream f(path);if(!f){std::cerr<<"log not found: "<<path<<'\n';return 2;}std::map<std::string,long long> c{{"DEBUG",0},{"INFO",0},{"WARNING",0},{"ERROR",0},{"CRITICAL",0}};std::string line;long long total=0;while(std::getline(f,line)){++total;for(auto&[k,v]:c)if(line.find(k)!=std::string::npos){++v;break;}}std::cout<<"total="<<total<<'\n';for(auto&[k,v]:c)std::cout<<k<<'='<<v<<'\n';return 0;}
