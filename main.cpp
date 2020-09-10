#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QTranslator* translator = new QTranslator();
    if (translator->load("french.qm"))
        a.installTranslator(translator);

    MainWIndow w;
    w.show();

    return a.exec();
}
