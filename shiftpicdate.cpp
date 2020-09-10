#include "shiftpicdate.h"


std::string spdFunc::getExifDate(const std::string &sFilename) {
   std::string Res("invalid metadata");

   if (fs::exists(sFilename) && test_ext(sFilename)) {

           Exiv2::Image::AutoPtr pImg;

           try {
               pImg=Exiv2::ImageFactory::open(sFilename);
               assert(pImg.get() != 0);
               pImg->readMetadata();
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
       else std::cerr << "- spdFunc::getExifDate(): File does not exist.\n";
   return Res;
}


bool spdFunc::setExifDate(const std::string &sFilename, const size_t Diff, bool bIsDST) {
        if (test_ext(sFilename)) {
                const std::string dateTimeFormat{ "%Y:%m:%d %H:%M:%S" };

                Exiv2::Image::AutoPtr pImg;

                try {
                    pImg=Exiv2::ImageFactory::open(sFilename);
                    assert(pImg.get() != 0);
                    pImg->readMetadata();

                    Exiv2::ExifData &exifData = pImg->exifData();

                    std::stringstream ssS(exifData["Exif.Image.DateTime"].toString());

                    std::tm dt={ };
                    if (bIsDST) dt.tm_isdst=bIsDST;

                    // chaine vers epoch
                    ssS >> std::get_time(&dt, dateTimeFormat.c_str());
                    std::time_t iEpoch=std::mktime(&dt);

                    // decalage
                    iEpoch+=Diff;

                    ssS.clear();

                    // epoch vers chaine
                    ssS << std::put_time(std::localtime(&iEpoch), (dateTimeFormat).c_str());

                    exifData["Exif.Image.DateTime"]=ssS.str();

                    pImg->setExifData(exifData);
                    pImg->writeMetadata();
                    pImg->readMetadata();
                }
                catch (Exiv2::AnyError& e) {
                    std::cerr << "- spdFunc::setExifDate(): " << e << ".\n";
                }
                catch (...) {
                    std::cerr << "- spdFunc::setExifDate(): error with " << sFilename << ".\n";
                }
                pImg.release();
            }
            else std::cerr << "- spdFunc::setExifDate(): File does not exist.\n";
    return true;
}


std::string spdFunc::stoyear(long long t) {
    std::stringstream ssS;

    constexpr long long M=60;
    constexpr long long H=60*M;
    constexpr long long D=24*H;
    constexpr long long Y=365*D;

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

std::string spdFunc::shiftTimestamp(const std::string &sTimestamp, long long t, bool bIsDST) { 
    const std::string dateTimeFormat{ "%Y:%m:%d %H:%M:%S" };    
    std::stringstream ssS(sTimestamp);
    
    std::tm dt={ };
    if (bIsDST) dt.tm_isdst=bIsDST;
    
    ssS >> std::get_time(&dt, dateTimeFormat.c_str());
    std::time_t iEpoch=std::mktime(&dt);
    
    iEpoch+=t;
    
    ssS.clear();
    ssS << std::put_time(std::localtime(&iEpoch), (dateTimeFormat).c_str());
    
    return ssS.str();
}
