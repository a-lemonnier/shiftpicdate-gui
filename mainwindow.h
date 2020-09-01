#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QDir>
#include <QFileDialog>
#include <QThread>
#include <QDebug>
#include <QTranslator>
#include <QMessageBox>
#include <QMetaType>
#include <QScrollBar>
#include <QTimer>

#include <filesystem>
#include <algorithm>
#include <memory>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWIndow; }
QT_END_NAMESPACE


class MainWIndow : public QMainWindow
{
    Q_OBJECT
    QThread lFThread;

public:
    MainWIndow(QWidget *parent = nullptr);
    ~MainWIndow();

    void setLogtextTh(const std::string &msg);
    void setLogtext(const std::string &msg);

    void getfileList();

public slots:
    void update_progressBar_value(int v);
    void update_Log_value(QString);
    void update_Log();

private slots:
    void on_bBrowse_clicked();
    void on_bRun_clicked();
    void on_bDST_clicked(bool checked);
    void on_bTest_clicked();
    void on_cBWrap_clicked(bool checked);
    void on_cBQuiet_clicked(bool checked);
    void on_bReset_clicked();
    void on_rBInfo_clicked();

    void on_bZoomIn_clicked();
    void on_bZoomOut_clicked();

    void on_sBDay_valueChanged(int val);
    void on_sBYear_valueChanged(int val);
    void on_sBSec_valueChanged(int arg1);
    void on_sBMin_valueChanged(int arg1);
    void on_sBHour_valueChanged(int arg1);


    void on_cBScroll_clicked(bool checked);

private:
    Ui::MainWIndow *ui;

    QTimer *timer;

    QString SLog;
    std::vector<QString> QSLog;

    std::string sFilename;

    long long DeltaY;
    long long DeltaD;
    long long DeltaH;
    long long DeltaM;
    long long DeltaS;

    long long DeltaT;

    bool isDST;
    bool isQuiet;
    bool isAutoScroll;

    int iZoom;

    int computeDeltaT();

signals:
    void LogTimout();

};


class fileList: public QObject {
    Q_OBJECT

public:

    explicit fileList(QObject *parent=Q_NULLPTR): QObject(parent) { }
    virtual ~fileList() { }

    void setfileName(std::string &str);

public slots:
    void getList();

signals:
    void fLProgress(int);
    void sendstdStr(QString);
    void finished();

private:
    std::string fileName;

    std::string sList;
    std::vector<std::string> vsList;

};




#endif // MAINWINDOW_H
