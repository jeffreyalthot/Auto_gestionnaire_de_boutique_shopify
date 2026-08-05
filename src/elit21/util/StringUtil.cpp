#include "elit21/util/StringUtil.h"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
namespace elit21::util {
std::string trim(std::string v){ auto n=[](unsigned char c){return !std::isspace(c);}; v.erase(v.begin(),std::find_if(v.begin(),v.end(),n)); v.erase(std::find_if(v.rbegin(),v.rend(),n).base(),v.end()); return v; }
std::string lower(std::string v){ std::transform(v.begin(),v.end(),v.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));}); return v; }
std::string upper(std::string v){ std::transform(v.begin(),v.end(),v.begin(),[](unsigned char c){return static_cast<char>(std::toupper(c));}); return v; }
std::string replaceAll(std::string v,const std::string& f,const std::string& t){ if(f.empty()) return v; for(std::size_t p=0;(p=v.find(f,p))!=std::string::npos;p+=t.size()) v.replace(p,f.size(),t); return v; }
std::string urlEncode(const std::string& s){ std::ostringstream o; o<<std::uppercase<<std::hex; for(unsigned char c:s){ if(std::isalnum(c)||c=='-'||c=='_'||c=='.'||c=='~') o<<c; else o<<'%'<<std::setw(2)<<std::setfill('0')<<static_cast<int>(c); } return o.str(); }
std::string urlDecode(const std::string& s){ std::string out; out.reserve(s.size()); auto hex=[](char c)->int{ if(c>='0'&&c<='9') return c-'0'; if(c>='a'&&c<='f') return c-'a'+10; if(c>='A'&&c<='F') return c-'A'+10; return -1; }; for(std::size_t i=0;i<s.size();++i){ if(s[i]=='%'&&i+2<s.size()){ int hi=hex(s[i+1]),lo=hex(s[i+2]); if(hi>=0&&lo>=0){ out.push_back(static_cast<char>((hi<<4)|lo)); i+=2; continue; } } if(s[i]=='+') out.push_back(' '); else out.push_back(s[i]); } return out; }
std::string join(const std::vector<std::string>& v,const std::string& sep){ std::ostringstream o; for(std::size_t i=0;i<v.size();++i){ if(i) o<<sep; o<<v[i]; } return o.str(); }
bool containsInsensitive(const std::string& a,const std::string& b){ return lower(a).find(lower(b))!=std::string::npos; }
std::string sanitizeTerminal(std::string v){ v.erase(std::remove_if(v.begin(),v.end(),[](unsigned char c){return c<32 && c!='\n' && c!='\t';}),v.end()); return v; }
}
