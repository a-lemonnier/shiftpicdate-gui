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
#include <cassert>
#include <tuple>

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
    long getExifEpoch(const std::string &sFilename);
    std::pair<std::string, long> getExifDateEpoch(const std::string &sFilename);
    bool setExifDate(const std::string &sFilename, const size_t Diff, bool bIsDST);

    long fileNb(const fs::path &path);

    std::tuple<long, long, long, long, long, long> decompEpoch(long t);
    std::string stoyear(long t);
    std::string shiftTimestamp(const std::string &sTimestamp, long t, bool bIsDST);
    
    bool test_ext(const std::string &sS);

    const std::string dateTimeFormat{ "%Y:%m:%d %H:%M:%S" };
}

// ----------------------------------------------------


#endif // shiftpicdate.h
