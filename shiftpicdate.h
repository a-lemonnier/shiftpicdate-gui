#ifndef SHIFTPICDATE_H
#define SHIFTPICDATE_H

#include <iostream>
#include <cmath>

#include <vector>

#include <algorithm>

#include <string>
#include <iomanip>
#include <sstream>
#include <locale>

#include <chrono>

#include <thread>
#include <future>

// ----------------------------------------------------
#include <exiv2/exiv2.hpp>

#include <boost/program_options.hpp>

#if __has_include (<boost/timer/timer.hpp>)
#include <boost/timer/timer.hpp> 
#define HAS_BOOST_TIMER /**< boost::timer availability */
#endif

#if __has_include (<filesystem>)
#include <filesystem>
#define FS_STD /**< std::filesystem availability (C++17) */
namespace fs = std::filesystem;
#elif __has_include (<experimental/filesystem>) && !__has_include (<filesystem>)
#include <experimental/filesystem>
#define FS_STDEXP /**< std::experimental::filesystem availability */
namespace fs = std::experimental::filesystem;
#elif __has_include(<boost/filesystem.hpp>) && !__has_include (<filesystem>) && !__has_include (<experimental/filesystem>)
#include <boost/filesystem.hpp>
#define FS_BOOST /**< boost::filesystem availability */
namespace fs = boost::filesystem;
#else
#error "No filesystem header found"
#endif
// ----------------------------------------------------

// ----------------------------------------------------
#define EXTENSION_LIST {".JPEG", ".jpeg", \
                        ".JPG",  ".jpg",  \
                        ".CR2",  ".cr2"  }

bool bQuiet;
bool bIsDST;
static const std::string dateTimeFormat{ "%Y:%m:%d %H:%M:%S" };
// ----------------------------------------------------

// Proto ----------------------------------------------
std::string stoyear(long long t);
inline void shift(std::vector<std::string> vList, long long Diff);
inline void shift_q(std::vector<std::string> vList, long long Diff);
inline bool test_ext(const std::string &sS);
// ----------------------------------------------------



// ----------------------------------------------------
// ----------------------------------------------------
// ----------------------------------------------------

std::string stoyear(long long t) {
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

void shift(std::vector<std::string> vList, long long Diff) {
    
    for(const auto &sFile: vList)
        if (test_ext(sFile)) {
            
                std::cout << "-> " << sFile << "\n";
                
                auto pImg=Exiv2::ImageFactory::open(sFile);

                pImg->readMetadata();
                
                Exiv2::ExifData &exifData = pImg->exifData();
                
                std::stringstream ssS(exifData["Exif.Image.DateTime"].toString());
                
                std::tm dt={ };
                if (bIsDST) dt.tm_isdst=bIsDST;
                
                ssS >> std::get_time(&dt, dateTimeFormat.c_str());
                
                std::time_t iEpoch=std::mktime(&dt);
                                
                iEpoch+=Diff;
                
                ssS.clear();
                ssS << std::put_time(std::localtime(&iEpoch), (dateTimeFormat).c_str());
                        
                exifData["Exif.Image.DateTime"]=ssS.str();
                    
                pImg->setExifData(exifData);
                pImg->writeMetadata();
                pImg->readMetadata();
            }
            else
                std::cerr << "/!\\ File does not exists.\n";
}

void shift_q(std::vector<std::string> vList, long long Diff) {
    
    for(const auto &sFile: vList)
        if (test_ext(sFile)) {
            
            if (fs::exists(sFile)) {
                
                auto pImg=Exiv2::ImageFactory::open(sFile);
                
                   pImg->readMetadata();
                
                Exiv2::ExifData &exifData = pImg->exifData();
                
                std::stringstream ssS(exifData["Exif.Image.DateTime"].toString());
                std::tm dt={ };
                
                if (bIsDST) dt.tm_isdst=bIsDST;
                
                ssS >> std::get_time(&dt, dateTimeFormat.c_str());
                
                std::time_t iEpoch=std::mktime(&dt);
                                
                iEpoch+=Diff;
                
                ssS.clear();
                ssS << std::put_time(std::localtime(&iEpoch), (dateTimeFormat).c_str());
                        
                exifData["Exif.Image.DateTime"]=ssS.str();
                    
                pImg->setExifData(exifData);
                pImg->writeMetadata();
                pImg->readMetadata();
            }
            else
                std::cerr << "/!\\ File does not exists.\n";
        }
}

bool test_ext(const std::string &sS) { 
    bool res=false;
    for(const auto &ext: EXTENSION_LIST)
        res|=(sS.find(ext)!=std::string::npos);
    return res;
}

#endif // shiftpicdate.h
