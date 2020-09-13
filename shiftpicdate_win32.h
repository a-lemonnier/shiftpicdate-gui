#ifndef SHIFTPICDATE_WIN32_H
#define SHIFTPICDATE_WIN32_H

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
#include <tuple>


#if defined(_WIN32) || defined(WIN32)
#include <stdio.h>
#include <tchar.h>
#include <atlbase.h>
#include <Wincodec.h>
#include <wincodecsdk.h>
#include <SDKDDKVer.h>
#pragma comment (lib,"Windowscodecs.lib")
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

#define EXTENSION_LIST {".JPEG",  ".jpeg", \
                        ".JPG",   ".jpg",  \
                        ".CR2",   ".cr2",  \
                        ".CR3",   ".cr3",  \
                        ".CR2",   ".cr2",  \
                        ".DCR",   ".dcr",  \
                        ".ARW",   ".arw",  \
                        ".RAW",   ".raw",  \
                        ".BRAW",  ".braw", \
                        ".PNG",   ".png"   }

// ----------------------------------------------------

namespace spdFunc {
    std::string getExifDate(const std::string &sFilename);
//     long getExifEpoch(const std::string &sFilename);
//     std::pair<std::string, long> getExifDateEpoch(const std::string &sFilename);
    bool setExifDate(const std::string &sFilename, long Diff, bool bIsDST);

    std::string shiftTimestamp(const std::string &sTimestamp, long t, bool bIsDST);
    long fileNb(const fs::path &path);

    std::tuple<long, long, long, long, long, long> decompEpoch(long t);
    std::string stoyear(long t);
    void test();
    bool test_ext(const std::string &sS);

}

// ----------------------------------------------------



std::string spdFunc::getExifDate(const std::string &sFilename);
long spdFunc::getExifEpoch(const std::string &sFilename);
std::pair<std::string, long> spdFunc::getExifDateEpoch(const std::string &sFilename);
bool spdFunc::setExifDate(const std::string &sFilename, long Diff, bool bIsDST=false);

std::string spdFunc::shiftTimestamp(const std::string &sTimestamp, long t, bool bIsDST=false);

long spdFunc::fileNb(const fs::path &path);

std::tuple<long, long, long, long, long, long> spdFunc::decompEpoch(long t);
std::string spdFunc::stoyear(long t);
void spdFunc::test();
bool spdFunc::test_ext(const std::string &sS);


#endif // shiftpicdate_win32.h
