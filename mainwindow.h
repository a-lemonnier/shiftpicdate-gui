#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define APPNAME "shiftpicdate-gui"

constexpr int VER_MAJOR=0;
constexpr int VER_MINOR=3;
constexpr int VER_REV=4;

constexpr int IOSLEEP=75; // wait 75ms between each RW
constexpr int BLINK_NB=5; // blink button 5 times

#include <Qt>
#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QThread>
#include <QtGlobal>
#include <QTimer>
#include <QElapsedTimer>
#include <QDir>
#include <QFileDialog>
#include <QTreeView>
#include <QListView>
#include <QTranslator>
#include <QMessageBox>
#include <QMetaType>
#include <QGraphicsEffect>
#include <QImageReader>
#include <QGraphicsView>
#include <QPixmap>
#include <QTransform>
#include <QMatrix>
#include <QTextStream>
#include <QPainter>
#include <QPainterPath>
#include <QRect>
#include <QDateTime>
#include <QSysInfo>
#include <QProcess>
#include <QRadioButton>
#include <QLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QList>

#include <QMouseEvent>
#include <QEvent>
#include <QFont>
#include <QSystemTrayIcon>

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>

#if defined(_WIN32) || defined(WIN32)
#include <QWinThumbnailToolBar>
#include <QWinThumbnailToolButton>
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#endif

#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QBarCategoryAxis>
#include <QValueAxis>

#include <ctime>
#include <algorithm>
#include <cassert>
#include <random>
#include <tuple>
#include <set>
#include <map>
#include <regex>
#include <exception>
#include <string>

#define VER_STR std::to_string(VER_MAJOR)+"." + \
                std::to_string(VER_MINOR)+"." + \
                std::to_string(VER_REV)
#define VER_QSTR QString::fromStdString(VER_STR)

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

#if defined(_WIN32) || defined(WIN32)
#include "shiftpicdate_win32.h"
#endif

#if defined(__linux__)
#include "shiftpicdate.h"
#endif

#include "fsdialog.h"
#include "data.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWIndow; }
QT_END_NAMESPACE

class MainWIndow : public QMainWindow {
    Q_OBJECT

    QThread lFThread;

public:
    MainWIndow(QWidget *parent = nullptr);
    ~MainWIndow();

    void setLogtextTh(const std::string &msg);
    void setLogtext(const std::string &msg);
    void setLogtextErr(const std::string &msg);

    void getfileList();

    void setvsList(std::vector<std::string> &);
    std::vector<std::string> getvsList();
    void startSlideshow();

    void initTaskBar();

    void checkForUpdate();

    enum Lang { FR, EN };
    enum Theme {DARK, LIGHT, QT, BLUE};

private slots:
    void on_bBrowse_clicked();
    void on_bRun_clicked();
    void on_bDST_clicked(bool checked);
    void on_bTest_clicked();
    void on_bWrap_clicked();
    void on_bQuiet_clicked();
    void on_bReset_clicked();
    void on_bZoomIn_clicked();
    void on_bZoomOut_clicked();
    void on_bScroll_clicked();
    void on_bHidepic_clicked();
    void on_bRot_clicked();
    void on_bNext_clicked();
    void on_bPrev_clicked();
    void on_bStop_clicked();
    void on_bSelectfile_clicked();
    void on_bFlag_released();
    void on_bBrowseFile_clicked();

    void on_sBDay_valueChanged(int val);
    void on_sBYear_valueChanged(int val);
    void on_sBSec_valueChanged(int val);
    void on_sBMin_valueChanged(int val);
    void on_sBHour_valueChanged(int val);

    void on_cbYear_activated(const QString &);

    void on_rBInfo_clicked();

    void update_progressBar_value(int);
    void update_SSprogressBar_value(int);
    void update_Log_value(QString);
    void update_Log();
    void changePic();
    void get_fsDialog_vector(std::vector<std::string> &);
    void run_shift();
    void plotHist();
    void addEpoch(long);
    void replyFinished(QNetworkReply*);
    void getButtontoBlink();
    void getRButtontoBlink();

private:
    Ui::MainWIndow *ui;

    QtCharts::QChartView *chartView;
    QtCharts::QChart *Chart;
    QtCharts::QBarCategoryAxis *axisX;
    QtCharts::QValueAxis *axisY;

    QTranslator qTranslator;

    QTimer *timer, *timer_ss;

    QGraphicsBlurEffect qeBlur;

#if defined(_WIN32) || defined(WIN32)
    QWinTaskbarButton *tbarButton;
    QWinTaskbarProgress *tbarProgress;
    QWinThumbnailToolBar *tbarThumb;
#endif

    QString SLog;

    std::string sFilename;
    std::vector<std::string> vsList;
    std::vector<long> vEpoch;

    long DeltaY, DeltaD, DeltaH, DeltaM, DeltaS;
    long DeltaT;

    bool isDST;
    bool isQuiet;
    bool isAutoScroll;

    int iZoom;
    int iPicRot;

    int iSlideshowInterval;

    long picNb;
    long currentPic;

    long lHistCurYear;

    Theme curHistTheme;

    Lang selectedLang;

    bool hasUpdate;
    std::string lastRelease;

    bool autoWrap;
    bool isPicHidden;

    int computeDeltaT();
    void changeTaskbarOverlay(const QString &);
    void changeTaskbarPic(const QPixmap&);

    void blinkButton(QPushButton *button);
    void blinkRButton(QRadioButton *rbutton);

signals:
    void LogTimout();

protected:
    void changeEvent(QEvent*);
    void mousePressEvent(QMouseEvent *event);
};


class fileList: public QObject {
    Q_OBJECT

public:

    explicit fileList(QObject *parent=Q_NULLPTR): QObject(parent) {  }
    virtual ~fileList() { }

    void setfileName(std::string &str);

public slots:
    void getList();
    std::vector<std::string> getvsList() const;

signals:
    void fLProgress(int);
    void sendstdStr(QString);
    void sendEpoch(long);
    void finished();

private:
    std::string fileName;

    std::vector<std::string> vsList;
};

class runShift: public QObject {
  Q_OBJECT

public:
    explicit runShift(QObject *parent=Q_NULLPTR): QObject(parent), Diff(0), bIsDST(false) {  }
    virtual ~runShift() { }

    void setvsList(std::vector<std::string> &vsList);
    void setDiff(long t);
    void setDST(bool bIsDST=true);
    
public slots:
    void shift();

signals:
    void sendProgress(int);
    void sendstdStr(QString);
    void finished();

private:
   std::vector<std::string> vsList;

   long Diff;
   bool bIsDST;
   
};



#endif // MAINWINDOW_H
