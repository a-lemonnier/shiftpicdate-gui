#include "shiftpicdate_win32.h"


std::string spdFunc::getExifDate(const std::string &sFilename) {
   std::string Res="invalid metadata";
   if (test_ext(sFilename)) {

       std::wstring wsFilename=std::wstring(sFilename.begin(), sFilename.end());

       const WCHAR* wcFilename=wsFilename.c_str();

       UINT  size = 0;
       UINT  count = 0;
       ULONG_PTR gdiplusToken;

       GdiplusStartupInput gdiplusStartupInput;
       GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

       Bitmap* bitmap = new Bitmap(wcFilename);
       bitmap->GetPropertySize(&size, &count);

       size = bitmap->GetPropertyItemSize(PropertyTagDateTime);
       PropertyItem* propertyItem = (PropertyItem*)malloc(size);
       bitmap->GetPropertyItem(PropertyTagDateTime, size, propertyItem);

       std::stringstream ssS;


       assert(ssS << static_cast<char*>( propertyItem->value) && Res=ssS.str(););


        //std::cerr << "- Error while loading image exif.\n";

       free(propertyItem);
       propertyItem=NULL;

       delete bitmap;
   }
   else
       return "invalid file.";

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


long long spdFunc::fileNb(const fs::path &path) {
    long long n=-1;
    try { n=std::distance(fs::recursive_directory_iterator{path}, fs::recursive_directory_iterator{}); }
    catch (fs::filesystem_error &err) {std::cerr << "- spdFunc::fileNb(): error: "  << ": " << err.what() << "\n";}
    return n;
}
