#include "elit21/security/Crypto.h"
#include "elit21/util/StringUtil.h"
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <iomanip>
#include <sstream>
namespace elit21::crypto {
static std::string digest(const EVP_MD* md,const std::string& d,bool upper){EVP_MD_CTX* c=EVP_MD_CTX_new();unsigned char out[EVP_MAX_MD_SIZE];unsigned int n=0;EVP_DigestInit_ex(c,md,nullptr);EVP_DigestUpdate(c,d.data(),d.size());EVP_DigestFinal_ex(c,out,&n);EVP_MD_CTX_free(c);std::ostringstream s;s<<(upper?std::uppercase:std::nouppercase)<<std::hex<<std::setfill('0');for(unsigned i=0;i<n;++i)s<<std::setw(2)<<static_cast<int>(out[i]);return s.str();}
std::string md5Hex(const std::string& d,bool u){return digest(EVP_md5(),d,u);}std::string sha256Hex(const std::string& d){return digest(EVP_sha256(),d,false);}std::string base64Encode(const unsigned char* d,std::size_t n){std::string o(4*((n+2)/3),'\0');int l=EVP_EncodeBlock(reinterpret_cast<unsigned char*>(o.data()),d,static_cast<int>(n));o.resize(static_cast<std::size_t>(l));return o;}
std::string hmacSha256Base64(const std::string& k,const std::string& d){unsigned char out[EVP_MAX_MD_SIZE];unsigned int n=0;HMAC(EVP_sha256(),k.data(),static_cast<int>(k.size()),reinterpret_cast<const unsigned char*>(d.data()),d.size(),out,&n);return base64Encode(out,n);}std::string hmacSha256Hex(const std::string& k,const std::string& d){unsigned char out[EVP_MAX_MD_SIZE];unsigned int n=0;HMAC(EVP_sha256(),k.data(),static_cast<int>(k.size()),reinterpret_cast<const unsigned char*>(d.data()),d.size(),out,&n);std::ostringstream s;s<<std::hex<<std::setfill('0');for(unsigned i=0;i<n;++i)s<<std::setw(2)<<static_cast<int>(out[i]);return s.str();}bool constantTimeEquals(const std::string&a,const std::string&b){if(a.size()!=b.size())return false;unsigned char r=0;for(std::size_t i=0;i<a.size();++i)r|=static_cast<unsigned char>(a[i]^b[i]);return r==0;}
std::string randomHex(std::size_t n){std::string b(n,'\0');if(RAND_bytes(reinterpret_cast<unsigned char*>(b.data()),static_cast<int>(n))!=1)return{};std::ostringstream s;s<<std::hex<<std::setfill('0');for(unsigned char c:b)s<<std::setw(2)<<static_cast<int>(c);return s.str();}
std::string aliExpressTopSign(const std::map<std::string,std::string>& p,const std::string& secret){std::string base=secret;for(const auto&[k,v]:p)if(k!="sign"&&!v.empty())base+=k+v;base+=secret;return util::upper(md5Hex(base,false));}
}
