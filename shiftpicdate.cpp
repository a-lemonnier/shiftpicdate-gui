#include "shiftpicdate.h"


std::string spdFunc::getExifDate(const std::string &sFilename) {
   std::string Res("invalid metadata");

   if (fs::exists(sFilename) && test_ext(sFilename)) {

           Exiv2::Image::AutoPtr pImg;

           try {
               pImg=Exiv2::ImageFactory::open(sFilename);
               pImg->readMetadata();
               assert(pImg.get() != 0);
               Exiv2::ExifData &exifData=pImg->exifData();
               Res=exifData["Exif.Image.DateTime"].toString();
           }
           catch (Exiv2::AnyError& e) {
               std::cerr << "- spdFunc::getExifDate(): " << e << ".\n";
           }
           catch (...) {
               std::cerr << "- spdFunc::getExifDate(): error with " << sFilename << ".\n";
           }

           pImg.release();
       }
       else
           std::cerr << "- spdFunc::getExifDate(): File does not exist.\n";

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

