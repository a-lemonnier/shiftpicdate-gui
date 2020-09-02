#include "shiftpicdate_win32.h"


std::string spdFunc::getExifDate(const std::string &sFilename) {
   std::string Res;

   if (test_ext(sFilename)) {
       std::wstring wsFilename=std::wstring(sFilename.begin(), sFilename.end());

       const WCHAR* wcFilename=wsFilename.c_str();

       GdiplusStartupInput gdiplusStartupInput;

       ULONG_PTR gdiplusToken;
       GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

       UINT  size = 0;
       UINT  count = 0;

       Bitmap* bitmap = new Bitmap(wcFilename);

       bitmap->GetPropertySize(&size, &count);

       size = bitmap->GetPropertyItemSize(PropertyTagDateTime);
       PropertyItem* propertyItem = (PropertyItem*)malloc(size);
       bitmap->GetPropertyItem(PropertyTagDateTime, size, propertyItem);

       Res = static_cast<char*>( propertyItem->value);

       free(propertyItem);
       propertyItem=NULL;

       delete bitmap;
   }

   return Res;
}


bool spdFunc::setExifDate(const std::string &sFilename, const std::string &sTimestamp) {

    return true;
}


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

bool spdFunc::test_ext(const std::string &sS) {
    bool res=false;
    for(const auto &ext: EXTENSION_LIST)
        res|=(sS.find(ext)!=std::string::npos);
    return res;
}

