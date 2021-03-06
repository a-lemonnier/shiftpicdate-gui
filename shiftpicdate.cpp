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

long spdFunc::getExifEpoch(const std::string &sFilename) {
    long lEpoch=0;

    if (test_ext(sFilename)) {

            Exiv2::Image::AutoPtr pImg;

            try {
                pImg=Exiv2::ImageFactory::open(sFilename);
                assert(pImg.get() != 0);
                pImg->readMetadata();

                Exiv2::ExifData &exifData = pImg->exifData();

                std::stringstream ssS(exifData["Exif.Image.DateTime"].toString());

                std::tm dt={ };

                // chaine vers epoch
                ssS >> std::get_time(&dt, dateTimeFormat.c_str());
                lEpoch=std::mktime(&dt);
            }
            catch (Exiv2::AnyError& e) {
                std::cerr << "- spdFunc::getExifEpoch(): " << e << ".\n";
            }
            catch (...) {
                std::cerr << "- spdFunc::getExifEpoch(): error with " << sFilename << ".\n";
            }
            pImg.release();
    }
    else std::cerr << "- spdFunc::getExifEpoch(): File does not exist.\n";
    return lEpoch>=0 ? lEpoch : 0;
}

std::pair<std::string, long> spdFunc::getExifDateEpoch(const std::string &sFilename) {
    std::string Res("invalid metadata");
    long lEpoch=0;

    if (test_ext(sFilename)) {
            Exiv2::Image::AutoPtr pImg;

            try {
                pImg=Exiv2::ImageFactory::open(sFilename);
                assert(pImg.get() != 0);
                pImg->readMetadata();

                Exiv2::ExifData &exifData = pImg->exifData();

                Res=exifData["Exif.Image.DateTime"].toString();
                std::stringstream ssS(Res);

                std::tm dt={ };
                // chaine vers epoch
                ssS >> std::get_time(&dt, dateTimeFormat.c_str());
                lEpoch=std::mktime(&dt);
            }
            catch (Exiv2::AnyError& e) {
                std::cerr << "- spdFunc::getExifEpoch(): " << e << ".\n";
            }
            catch (...) {
                std::cerr << "- spdFunc::getExifEpoch(): error with " << sFilename << ".\n";
            }
            pImg.release();
    }
    else std::cerr << "- spdFunc::getExifEpoch(): File does not exist.\n";
    return {Res, lEpoch>=0 ? lEpoch : 0};
}

bool spdFunc::setExifDate(const std::string &sFilename, const size_t Diff, bool bIsDST) {
        if (test_ext(sFilename)) {
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

                    // epoch vers chaine
                    ssS.clear();
                    ssS << std::put_time(std::localtime(&iEpoch), (dateTimeFormat).c_str());

                    exifData["Exif.Image.DateTime"]=ssS.str();

                    pImg->setExifData(exifData);
                    pImg->writeMetadata();
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

std::tuple<long, long, long, long, long, long> spdFunc::decompEpoch(long t) {
    time_t Epoch(t);
    struct tm dt=*std::localtime(&Epoch);
    return {dt.tm_year +1900,
            dt.tm_mon+1,
            dt.tm_mday,
            dt.tm_hour,
            dt.tm_min,
            dt.tm_sec };
}

std::string spdFunc::stoyear(long t) {
    std::stringstream ssS;

    constexpr long M=60;
    constexpr long H=60*M;
    constexpr long D=24*H;
    constexpr long Y=365.24*static_cast<float>(D);

    long t0=-t;

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

long spdFunc::fileNb(const fs::path &path) {
    long n=-1;
    try { n=std::distance(fs::recursive_directory_iterator{path}, fs::recursive_directory_iterator{}); }
    catch (fs::filesystem_error &err) {std::cerr << "- spdFunc::fileNb(): error: "  << ": " << err.what() << "\n";}
    return n;
}

std::string spdFunc::shiftTimestamp(const std::string &sTimestamp, long t, bool bIsDST) { 
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


