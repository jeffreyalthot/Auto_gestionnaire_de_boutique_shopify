#include "elit21/security/Crypto.h"
#include <cstdlib>
#include <iostream>
int main(int argc,char**argv){std::size_t bytes=argc>1?static_cast<std::size_t>(std::max(16,std::atoi(argv[1]))):32;std::cout<<elit21::crypto::randomHex(bytes)<<'\n';return 0;}
