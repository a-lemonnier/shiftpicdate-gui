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

#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <objidl.h>
#include <strsafe.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#endif

#ifdef __unix__
#include <exiv2/exiv2.hpp>
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
    bool setExifDate(const std::string &sFilename, const size_t Diff, bool bIsDST);

    long long fileNb(const fs::path &path);

    std::string stoyear(long long t);
    std::string stoFullStdString(size_t t);
    void test();
    bool test_ext(const std::string &sS);
}

// ----------------------------------------------------



std::string spdFunc::getExifDate(const std::string &sFilename);
bool spdFunc::setExifDate(const std::string &sFilename, const size_t Diff, bool bIsDST);

long long spdFunc::fileNb(const fs::path &path);

std::string spdFunc::stoyear(long long t);
std::string spdFunc::stoFullStdString(size_t t);

void spdFunc::test();
bool spdFunc::test_ext(const std::string &sS);

#endif // shiftpicdate.h
