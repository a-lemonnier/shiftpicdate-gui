#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QCommandLineParser>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName(APPNAME);
    QCoreApplication::setApplicationVersion(VER_QSTR);

    QCommandLineParser parser;
    parser.setApplicationDescription(APPNAME);
    parser.addHelpOption();
    parser.addVersionOption();



    QCommandLineOption Year(QStringList() << "Y" << "year",
                           QCoreApplication::translate("main", "Years."),  "items", "0");
    QCommandLineOption Day(QStringList() << "D" << "day",
                           QCoreApplication::translate("main", "Days."), 0);
    QCommandLineOption Hour(QStringList() << "H" << "hour",
                           QCoreApplication::translate("main", "Hours."), 0);
    QCommandLineOption Minute(QStringList() << "M" << "month",
                           QCoreApplication::translate("main", "Minutes."), 0);
    QCommandLineOption Second(QStringList() << "S" << "second",
                           QCoreApplication::translate("main", "Seconds."), 0);

    parser.addOption(Year);
    parser.addOption(Day);
    parser.addOption(Hour);
    parser.addOption(Minute);
    parser.addOption(Second);

    parser.process(app);


    qDebug() << parser.value(Year);
    qDebug() << parser.value(Hour);

    if (parser.optionNames().empty()) {
        MainWIndow w;
        w.show();
        w.initTaskBar();
        return app.exec();
    }
    else {
        parser.showHelp();
    }


}
