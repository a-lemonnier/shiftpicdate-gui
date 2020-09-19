#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWIndow w;
    w.show();

    w.initTaskBar();

    return a.exec();
}
