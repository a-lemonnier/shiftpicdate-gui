#include "shiftpicdate.h"

std::string spdFunc::stoyear(long long t) {
    std::stringstream ssS;
    
    long long M=60;
    long long H=60*M;
    long long D=24*H;
    long long Y=365*D;
    
    long long t0=-t;
    
    if (t<0) 
        ssS << "-"
        << (  t0 / Y)               << "y "
        << (  t0 % Y) / D           << "d "
        << (( t0 % Y) % D) / H      << "h "
        << (((t0 % Y) % D) % H) / M << "m "
        << (((t0 % Y) % D) % H) % M << "s"; 
    else
        ssS << (  t / Y)           << "y "
        << (  t % Y) / D           << "d "
        << (( t % Y) % D) / H      << "h "
        << (((t % Y) % D) % H) / M << "m "
        << (((t % Y) % D) % H) % M << "s";
    
    return ssS.str();
}


void spdFunc::test() {
  auto img=Exiv2::ImageFactory::open("ico.svg");

}
