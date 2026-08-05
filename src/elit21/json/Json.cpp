#include "elit21/json/Json.h"
#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>
namespace elit21 {
Json::Json():object_(nullptr){}
Json::Json(json_object* o,bool own):object_(o){ if(object_ && !own) json_object_get(object_); }
Json::Json(const Json& o):object_(o.object_){ if(object_) json_object_get(object_); }
Json::Json(Json&& o) noexcept:object_(o.object_){o.object_=nullptr;}
Json& Json::operator=(const Json& o){ if(this!=&o){ if(object_)json_object_put(object_); object_=o.object_; if(object_)json_object_get(object_);} return *this; }
Json& Json::operator=(Json&& o) noexcept{ if(this!=&o){if(object_)json_object_put(object_);object_=o.object_;o.object_=nullptr;}return *this;}
Json::~Json(){if(object_)json_object_put(object_);}
Result<Json> Json::parse(const std::string& t){ json_tokener* tok=json_tokener_new(); json_object* o=json_tokener_parse_ex(tok,t.c_str(),static_cast<int>(t.size())); auto err=json_tokener_get_error(tok); json_tokener_free(tok); if(err!=json_tokener_success||!o) return Result<Json>::failure(json_tokener_error_desc(err)); return Result<Json>::success(Json(o,true)); }
Result<Json> Json::parseFile(const std::string& p){std::ifstream f(p,std::ios::binary);if(!f)return Result<Json>::failure("Impossible d'ouvrir: "+p);std::ostringstream s;s<<f.rdbuf();return parse(s.str());}
Json Json::object(){return Json(json_object_new_object(),true);} Json Json::array(){return Json(json_object_new_array(),true);}
bool Json::isNull()const{return !object_||json_object_is_type(object_,json_type_null);}
bool Json::isObject()const{return object_&&json_object_is_type(object_,json_type_object);}
bool Json::isArray()const{return object_&&json_object_is_type(object_,json_type_array);}
bool Json::isString()const{return object_&&json_object_is_type(object_,json_type_string);}
bool Json::isNumber()const{return object_&&(json_object_is_type(object_,json_type_double)||json_object_is_type(object_,json_type_int));}
bool Json::isBool()const{return object_&&json_object_is_type(object_,json_type_boolean);}
bool Json::contains(const std::string& k)const{json_object* v=nullptr;return isObject()&&json_object_object_get_ex(object_,k.c_str(),&v);}
Json Json::get(const std::string& k)const{json_object* v=nullptr;if(isObject()&&json_object_object_get_ex(object_,k.c_str(),&v))return Json(v,false);return Json();}
Json Json::at(std::size_t i)const{if(!isArray()||i>=size())return Json();return Json(json_object_array_get_idx(object_,i),false);}std::size_t Json::size()const{return isArray()?json_object_array_length(object_):0;}
std::string Json::stringValue(const std::string& f)const{return isString()?json_object_get_string(object_):f;}
std::string Json::scalarStringValue(const std::string& f)const{
    if(isString())return json_object_get_string(object_);
    if(object_&&json_object_is_type(object_,json_type_int))return std::to_string(json_object_get_int64(object_));
    if(object_&&json_object_is_type(object_,json_type_double)){std::ostringstream out;out.precision(17);out<<json_object_get_double(object_);return out.str();}
    if(isBool())return json_object_get_boolean(object_)!=0?"true":"false";
    return f;
}
double Json::numberValue(double f)const{return isNumber()?json_object_get_double(object_):f;}
std::int64_t Json::int64Value(std::int64_t f)const{return object_&&json_object_is_type(object_,json_type_int)?json_object_get_int64(object_):f;}
int Json::intValue(int f)const{
    if(!object_||!json_object_is_type(object_,json_type_int))return f;
    const auto value=json_object_get_int64(object_);
    if(value<std::numeric_limits<int>::min()||value>std::numeric_limits<int>::max())return f;
    return static_cast<int>(value);
}
bool Json::boolValue(bool f)const{return isBool()?json_object_get_boolean(object_)!=0:f;}
std::string Json::getString(const std::string& k,const std::string& f)const{return get(k).stringValue(f);}
std::string Json::getScalarString(const std::string& k,const std::string& f)const{return get(k).scalarStringValue(f);}
double Json::getNumber(const std::string& k,double f)const{return get(k).numberValue(f);}
std::int64_t Json::getInt64(const std::string& k,std::int64_t f)const{return get(k).int64Value(f);}
int Json::getInt(const std::string& k,int f)const{return get(k).intValue(f);}
bool Json::getBool(const std::string& k,bool f)const{return get(k).boolValue(f);}
void Json::set(const std::string& k,const std::string& v){if(!isObject())return;json_object_object_add(object_,k.c_str(),json_object_new_string(v.c_str()));}
void Json::set(const std::string& k,const char* v){set(k,std::string(v?v:""));}
void Json::set(const std::string& k,double v){if(isObject()&&std::isfinite(v))json_object_object_add(object_,k.c_str(),json_object_new_double(v));}
void Json::set(const std::string& k,std::int64_t v){if(isObject())json_object_object_add(object_,k.c_str(),json_object_new_int64(v));}
void Json::set(const std::string& k,int v){if(isObject())json_object_object_add(object_,k.c_str(),json_object_new_int(v));}void Json::set(const std::string& k,bool v){if(isObject())json_object_object_add(object_,k.c_str(),json_object_new_boolean(v));}void Json::set(const std::string& k,const Json& v){if(isObject()){json_object* c=v.object_?json_object_get(v.object_):json_object_new_null();json_object_object_add(object_,k.c_str(),c);}}void Json::push(const Json& v){if(isArray())json_object_array_add(object_,v.object_?json_object_get(v.object_):json_object_new_null());}
std::string Json::dump(bool pretty)const{return object_?json_object_to_json_string_ext(object_,pretty?JSON_C_TO_STRING_PRETTY:JSON_C_TO_STRING_PLAIN):"null";}
}
