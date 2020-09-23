#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QCommandLineParser>
#include <QStringList>

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

#include <string>

#include "shiftpicdate.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QTranslator translator;
    if (translator.load("french.qm"))
        app.installTranslator(&translator);

    QApplication::setApplicationName(APPNAME);
    QApplication::setApplicationVersion(VER_QSTR);

    QCommandLineParser parser;
    parser.setApplicationDescription(APPNAME);
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription(QApplication::translate("main", "Shift the EXIF date and time of a picture, "
                                                                     "or pictures of a given directory."));

    QCommandLineOption Directory(QStringList() << "d" << "dir",
                           QApplication::translate("main", "Specify to directory where find the pictures to shift."),"string");
    QCommandLineOption File(QStringList() << "f" << "file",
                           QApplication::translate("main", "Specify the file to shift (do not use -d option)."), "string");
    QCommandLineOption Year(QStringList() << "Y" << "year",
                           QApplication::translate("main", "Years."), "int", 0);
    QCommandLineOption Day(QStringList() << "D" << "day",
                           QApplication::translate("main", "Days."), "int", 0);
    QCommandLineOption Hour(QStringList() << "H" << "hour",
                           QApplication::translate("main", "Hours."), "int", 0);
    QCommandLineOption Minute(QStringList() << "M" << "minute",
                           QApplication::translate("main", "Minutes."), "int", 0);
    QCommandLineOption Second(QStringList() << "S" << "second",
                           QApplication::translate("main", "Seconds."), "int", 0);
    QCommandLineOption DST(QStringList() << "DST",
                           QApplication::translate("main", "Enable the DST."));

    parser.addOption(Directory);
    parser.addOption(File);
    parser.addOption(Year);
    parser.addOption(Day);
    parser.addOption(Hour);
    parser.addOption(Minute);
    parser.addOption(Second);
    parser.addOption(DST);

    parser.process(app);

    //const QStringList args = parser.positionalArguments();

    std::string sFilename=parser.value(File).toStdString();
    std::string sDirectory=parser.value(Directory).toStdString();

    long long Diff;
    Diff+=parser.value(Year).toLongLong()*3600*24*365;
    Diff+=parser.value(Day).toLongLong()*3600*24;
    Diff+=parser.value(Hour).toLongLong()*3600;
    Diff+=parser.value(Minute).toLongLong()*60;
    Diff+=parser.value(Second).toLongLong();

    bool bDST=!parser.isSet(DST);

    if (parser.optionNames().empty()) {
        MainWIndow w;
        w.show();
        w.initTaskBar();
        return app.exec();
    }


    if (!(parser.isSet(File) ^ parser.isSet(Directory))) {
        parser.showHelp();
        return EXIT_SUCCESS;
    }

    if (parser.isSet(Directory))   {
        if (fs::is_directory(sDirectory)) {
            std::vector<std::string> vList;
            for(const auto &sFile: fs::directory_iterator(sDirectory))
                if (spdFunc::test_ext(sFile.path().filename().string()))
                    vList.push_back(sFile.path().string());

            std::sort(vList.begin(), vList.end());

            std::cout << "- Number of files: " << vList.size() << ".\n";
            std::cout << "- Shift date by: " << Diff << "s -> "
                                             << spdFunc::stoyear(Diff) << ".\n";
            if (bDST) std::cout << "- DST is enabled.\n";

            std::cout << "- Start shifting...\n";
            for(const auto &sFile: vList) {
                std::cout << "\t-> " << sFile << "\n";
                spdFunc::setExifDate(sFile, Diff, bDST);
            }
            std::cout << "- Shifting done.\n";

            std::cout << "- New dates:\n";
            for(const auto &sFile: vList)
                std::cout << "\t-> " << sFile
                          << " - " << spdFunc::getExifDate(sFile) << ".\n";

            std::cout << "- All done.\n";
        }
        else {
            std::cerr << "- Directory does not exist: " << sDirectory << ".\n";
            return EXIT_FAILURE;
        }
    }
    else {
        if (fs::exists(sFilename) && spdFunc::test_ext(sFilename)) {
            std::cout << "- Shift date by: " << Diff << "s -> "
                                             << spdFunc::stoyear(Diff) << ".\n";
            if (bDST) std::cout << "- DST is enabled.\n";
            std::cout << "- Work with " << sFilename
                      << " - " << spdFunc::getExifDate(sFilename) << ".\n";
            spdFunc::setExifDate(sFilename, Diff, bDST);
            std::cout << "- New date: " << spdFunc::getExifDate(sFilename) << ".\n";

            std::cout << "- All done.\n";
        }
        else {
            std::cerr << "- File does not exist or is invalid: " << sFilename << ".\n";
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
