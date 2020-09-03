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


#if defined(_WIN32) || defined(WIN32)
#include <cassert>
#include <windows.h>
#include <objidl.h>
#include <strsafe.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
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
    bool setExifDate(const std::string &sFilename, const std::string &sTimestamp);

    long long fileNb(const fs::path &path);

    std::string stoyear(long long t);
    void test();
    bool test_ext(const std::string &sS);
}

// ----------------------------------------------------



std::string spdFunc::getExifDate(const std::string &sFilename);
bool spdFunc::setExifDate(const std::string &sFilename, const std::string &sTimestamp);

long long spdFunc::fileNb(const fs::path &path);

std::string spdFunc::stoyear(long long t);
void spdFunc::test();
bool spdFunc::test_ext(const std::string &sS);

#endif // shiftpicdate_win32.h
