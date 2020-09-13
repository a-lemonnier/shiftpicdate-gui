#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWIndow::MainWIndow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWIndow)
, qTranslator(QTranslator())
, SLog(tr("- Initialisation.\n"))
, sFilename(".")
, DeltaY(0)
, DeltaD(0)
, DeltaH(0)
, DeltaM(0)
, DeltaS(0)
, DeltaT(0)
, isDST(false)
, isQuiet(false)
, isAutoScroll(true)
, iPicRot(-90)
, iSlideshowInterval(1000)
, currentPic(0)
, selectedLang(Lang::EN) {
    ui->setupUi(this);
    this->setWindowTitle(QString::fromStdString("shiftpicdate-gui "+std::string(VER)));

    // Set translation --
    QString defaultLocale = QLocale::system().name();
    defaultLocale.truncate(defaultLocale.lastIndexOf('_'));

    if (defaultLocale.toStdString().find("fr") ||
        defaultLocale.toStdString().find("FR") ||
        defaultLocale.toStdString().find("Fr")) {
        this->selectedLang=Lang::FR;
        if (qTranslator.load("french.qm"))
          qApp->installTranslator(&qTranslator);
        this->setLogtext(tr("- Switch language to French.").toStdString());
        ui->bFlag->setIcon(QIcon(":/flag/fr.svg"));
        ui->retranslateUi(this);
    }
    else ui->bFlag->setIcon(QIcon(":/flag/eng.svg"));
    // ------------------

    ui->tBLog->setAcceptRichText(true);
    this->setLogtext("\n");
    
    // Enable widgets --
    ui->bTest->setHidden(true);

    ui->bRun->setEnabled(false);
    ui->bReset->setEnabled(false);
    ui->bSelectfile->setEnabled(false);
    
    ui->sBYear->setEnabled(true);
    ui->sBDay->setEnabled(true);
    ui->bDST->setEnabled(true);

    ui->bRot->setEnabled(false);
    ui->bNext->setEnabled(false);
    ui->bPrev->setEnabled(false);
    ui->bStop->setEnabled(false);

    ui->lEPath->setEnabled(false);
    // -----------------

    // Set values --
    ui->bDST->setChecked(this->isDST);
    
    ui->cBScroll->setChecked(true);
    ui->cBWrap->setChecked(true);
    
    ui->sBYear->setValue(this->DeltaY);
    ui->sBDay->setValue(this->DeltaD);
    ui->sBHour->setValue(this->DeltaH);
    ui->sBMin->setValue(this->DeltaM);
    ui->sBSec->setValue(this->DeltaS);
    // -------------

    // Set Styles --
    ui->lEPath->setStyleSheet(spdStyle::lEPathRed);
    ui->bBrowse->setStyleSheet(spdStyle::bBrowseRed);
    ui->tBLog->setStyleSheet("");

    ui->tBLog->verticalScrollBar()->setStyleSheet("");
    ui->tBLog->verticalScrollBar()->setStyleSheet("");

    ui->sBYear->setStyleSheet(spdStyle::TimeField);
    ui->sBDay->setStyleSheet(spdStyle::TimeField);
    ui->sBHour->setStyleSheet(spdStyle::TimeField);
    ui->sBMin->setStyleSheet(spdStyle::TimeField);
    ui->sBSec->setStyleSheet(spdStyle::TimeField);
    // -------------

    // Blur --
    qeBlur.setEnabled(false);
    qeBlur.setBlurRadius(2);
    this->centralWidget()->setGraphicsEffect(&qeBlur);
    // -------

    // Histo --
    this->bSerie=new QtCharts::QBarSeries;
    this->Chart=new QtCharts::QChart();
    this->chartView = new QtCharts::QChartView(this->Chart);

    this->bSerie->setUseOpenGL(true);

    this->Chart->setAnimationOptions(QtCharts::QChart::AllAnimations);
    this->Chart->setTheme(QtCharts::QChart::ChartThemeDark);
    this->Chart->setToolTip(tr("Time line."));

    this->chartView->setRenderHint(QPainter::Antialiasing);
    this->chartView->setMaximumHeight(125);

    ui->hlBarChart->setSizeConstraint(QLayout::SetFixedSize);
    ui->hlBarChart->addWidget(this->chartView);

    this->plotHist(); // put this after getting file list
    // --------

    // Define some connections
    connect(ui->tBLog->horizontalScrollBar(), &QScrollBar::sliderMoved, [this]() {ui->tBLog->setStyleSheet("");});
    connect(ui->tBLog->verticalScrollBar(),   &QScrollBar::sliderMoved, [this]() {ui->tBLog->setStyleSheet("");});
}

MainWIndow::~MainWIndow() { delete ui; }

void MainWIndow::on_bBrowse_clicked() {
    qeBlur.setEnabled(true);

    ui->lEPath->setStyleSheet("");
    ui->bBrowse->setStyleSheet("");
    this->vsList.clear();

    QFileDialog dialog(this);

#if defined(__linux__)
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
#else
    dialog.setFileMode(QFileDialog::DirectoryOnly);
#endif

    dialog.setViewMode(QFileDialog::List);
    
    // See on QT forum ***
    QListView *list = dialog.findChild<QListView*>("listView");
    if (list)  list->setSelectionMode(QAbstractItemView::MultiSelection);
    QTreeView *tree = dialog.findChild<QTreeView*>();
    if (tree)  tree->setSelectionMode(QAbstractItemView::MultiSelection);
    // ***
    
    QStringList fileNames={""};

    if (dialog.exec()) fileNames=dialog.selectedFiles();

    ui->lEPath->setText(fileNames[0]);
    ui->lEPath->setEnabled(false);
    
    this->setLogtext(tr("- Path selected: ").toStdString());
    for(const auto &str: fileNames)
        this->setLogtext(str.toUtf8().toStdString()+"\n");
    
    if (fs::is_directory(fileNames[0].toUtf8().toStdString())) {
        this->sFilename=fileNames[0].toUtf8().toStdString()+"/";

        // if one pic has been selected then display it
        long llFilenb=spdFunc::fileNb(this->sFilename);

        if (llFilenb>-1) {
            if (llFilenb==1 && spdFunc::test_ext((*fs::recursive_directory_iterator(this->sFilename)).path().string())) {

                QString oneFilename=QString::fromStdString((*fs::recursive_directory_iterator(this->sFilename)).path().string());

                this->setLogtext(tr("- Load image ").toStdString()+oneFilename.toStdString()+": ");

                ui->bReset->setEnabled(true);

                // Read pictures
                QImageReader* imgr=new QImageReader(oneFilename);
                imgr->read();

                this->setLogtext(imgr->errorString().toStdString()+"\n");

                // Rescale pictures
                QPixmap pmap(oneFilename);
                ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(),ui->picLabel->size().width(),Qt::KeepAspectRatio));

                delete imgr;
            }
            else {
                this->setEnabled(false);
                this->getfileList();
                this->setEnabled(true);
            }
        }
        else
            this->setLogtext(tr("- \u25EC Cannot process with this path: ").toStdString()+this->sFilename+".\n");
    }
    qeBlur.setEnabled(false);
}

void MainWIndow::on_bRun_clicked() {
    ui->sBYear->setStyleSheet(spdStyle::TimeField);
    ui->sBDay->setStyleSheet(spdStyle::TimeField);
    ui->sBHour->setStyleSheet(spdStyle::TimeField);
    ui->sBMin->setStyleSheet(spdStyle::TimeField);
    ui->sBSec->setStyleSheet(spdStyle::TimeField);

    ui->bQuit->setEnabled(true);
    ui->bBrowse->setEnabled(false);
    ui->bReset->setEnabled(false);
    ui->sBYear->setEnabled(false);
    ui->sBDay->setEnabled(false);
    ui->sBHour->setEnabled(false);
    ui->sBMin->setEnabled(false);
    ui->sBSec->setEnabled(false);

    this->run_shift();

    ui->bQuit->setEnabled(true);
    ui->bBrowse->setEnabled(true);
    ui->bReset->setEnabled(true);
    ui->sBYear->setEnabled(true);
    ui->sBDay->setEnabled(true);
    ui->sBHour->setEnabled(true);
    ui->sBMin->setEnabled(true);
    ui->sBSec->setEnabled(true);

    ui->tBLog->setStyleSheet("");
}

void MainWIndow::on_bDST_clicked(bool checked) {
    if (!checked) {
        this->isDST=false;
        this->setLogtext(tr("- DST is disable.\n").toStdString());
    }
    else {
        this->isDST=true;
        this->setLogtext(tr("- DST is enable: +1h.\n").toStdString());
    }
}

void MainWIndow::on_bZoomIn_clicked()  { ui->tBLog->zoomIn(1);  }
void MainWIndow::on_bZoomOut_clicked() { ui->tBLog->zoomOut(1); }
void MainWIndow::on_bTest_clicked() { ui->bReset->setEnabled(true); }
void MainWIndow::on_cBHidepic_clicked(bool checked) { ui->picLabel->setHidden(checked); }
void MainWIndow::on_bStop_clicked() { if (this->timer_ss->isActive()) timer_ss->stop(); }

void MainWIndow::on_bRot_clicked() {
    if (!this->timer_ss->isActive()) {
        this->iPicRot=90;

        QPixmap cPmap(*ui->picLabel->pixmap()); // keep this till QT 6 or 7...
        
        // QTBUG-48701 enum ReturnByValueConstant { ReturnByValue };
        // ReturnByValue not member of Qt: ver < 5.15.0 ?
        // QPixmap cPmap(ui->picLabel->pixmap(Qt::ReturnByValue);

        QTransform transform(QTransform().rotate(this->iPicRot));
        cPmap = cPmap.transformed(transform);

        ui->picLabel->setPixmap(cPmap);
        this->iPicRot=0;
    }
    else this->iPicRot=90;
}


void MainWIndow::on_cBWrap_clicked(bool checked) {
    if (!checked) {
        ui->tBLog->setLineWrapMode(QTextEdit::NoWrap);
        this->setLogtext(tr("- Auto wrap off.\n").toStdString());
    }
    else {
        ui->tBLog->setLineWrapMode(QTextEdit::WidgetWidth);
        this->setLogtext(tr("- Auto wrap on.\n").toStdString());
    }
}

void MainWIndow::on_cBQuiet_clicked(bool checked) {
    this->isQuiet=checked;
    if (!checked) this->setLogtext(tr("- Toggle verbosity on.\n").toStdString());
}

void MainWIndow::on_cBScroll_clicked(bool checked) {
    this->isAutoScroll=checked;
    if (!this->isQuiet) {
        if (checked) this->setLogtext(tr("- Toggle auto scroll on.\n").toStdString());
        else         this->setLogtext(tr("- Toggle auto scroll off.\n").toStdString());
    }
}

void MainWIndow::on_bReset_clicked() {
    ui->bRun->setEnabled(true);
    ui->sBYear->setEnabled(true);
    ui->sBDay->setEnabled(true);
    ui->bDST->setEnabled(true);
    ui->lEPath->setEnabled(true);
    
    ui->bDST->setChecked(this->isDST);
    ui->cBHidepic->setChecked(false);
    
    QString savedStrp="<p><span style=\" font-weight:600; color:#aa0000;\">";
    QString savedStr=tr("No picture selected.");
    QString savedStrs="</span></p>";
    
    savedStr=savedStrp+savedStr+savedStrs;
    
    ui->picLabel->clear();
    ui->picLabel->setText(savedStr);
    
    this->sFilename="./";
    ui->lEPath->setText("./");
    
    this->SLog.clear();
    this->SLog="\0";
    this->setLogtextTh("\0");
    ui->tBLog->clear();
    
    this->DeltaY=0;
    this->DeltaD=0;
    this->DeltaH=0;
    this->DeltaM=0;
    this->DeltaS=0;
    
    this->computeDeltaT();
    
    ui->sBYear->setValue(this->DeltaY);
    ui->sBDay->setValue(this->DeltaD);
    ui->sBHour->setValue(this->DeltaH);
    ui->sBMin->setValue(this->DeltaM);
    ui->sBSec->setValue(this->DeltaS);

    this->timer_ss->stop();
    ui->picLabel->setToolTipDuration(-1);
    ui->statusbar->showMessage("Reset...");

    ui->lEPath->setStyleSheet(spdStyle::lEPathRed);
    ui->bBrowse->setStyleSheet(spdStyle::bBrowseRed);
    ui->bRun->setStyleSheet("");
    ui->tBLog->setStyleSheet("");

    ui->sBYear->setStyleSheet(spdStyle::TimeField);
    ui->sBDay->setStyleSheet(spdStyle::TimeField);
    ui->sBHour->setStyleSheet(spdStyle::TimeField);
    ui->sBMin->setStyleSheet(spdStyle::TimeField);
    ui->sBSec->setStyleSheet(spdStyle::TimeField);
}

void MainWIndow::on_sBDay_valueChanged(int val) {
    ui->sBYear->setStyleSheet(spdStyle::TimeField);
    ui->sBDay->setStyleSheet(spdStyle::TimeField);
    ui->sBHour->setStyleSheet(spdStyle::TimeField);
    ui->sBMin->setStyleSheet(spdStyle::TimeField);
    ui->sBSec->setStyleSheet(spdStyle::TimeField);

    ui->bRun->setEnabled(true);
    
    this->DeltaD=val;
    this->computeDeltaT();
    this->setLogtext(tr("-> Shift by ").toStdString()
                     +spdFunc::stoyear( this->computeDeltaT())
                     +tr(" or ").toStdString()
                     +std::to_string(this->computeDeltaT())
                     +"s.\n");
}

void MainWIndow::on_sBYear_valueChanged(int val) { 
    ui->sBYear->setStyleSheet(spdStyle::TimeField);
    ui->sBDay->setStyleSheet(spdStyle::TimeField);
    ui->sBHour->setStyleSheet(spdStyle::TimeField);
    ui->sBMin->setStyleSheet(spdStyle::TimeField);
    ui->sBSec->setStyleSheet(spdStyle::TimeField);

    ui->bRun->setEnabled(true);
    
    this->DeltaY=val;
    this->computeDeltaT();
    
    this->setLogtext(tr("-> Shift by ").toStdString()
                     +spdFunc::stoyear( this->computeDeltaT())
                     +tr(" or ").toStdString()
                     +std::to_string(this->computeDeltaT())
                     +"s.\n");
}

void MainWIndow::on_sBSec_valueChanged(int val) {
    ui->sBYear->setStyleSheet(spdStyle::TimeField);
    ui->sBDay->setStyleSheet(spdStyle::TimeField);
    ui->sBHour->setStyleSheet(spdStyle::TimeField);
    ui->sBMin->setStyleSheet(spdStyle::TimeField);
    ui->sBSec->setStyleSheet(spdStyle::TimeField);

    ui->bRun->setEnabled(true);
    
    this->DeltaS=val;
    this->computeDeltaT();
    
    this->setLogtext(tr("-> Shift by ").toStdString()
                     +spdFunc::stoyear( this->computeDeltaT())
                     +" or "
                     +std::to_string(this->computeDeltaT())
                     +"s.\n");
}

void MainWIndow::on_sBMin_valueChanged(int val) {
    ui->sBYear->setStyleSheet(spdStyle::TimeField);
    ui->sBDay ->setStyleSheet(spdStyle::TimeField);
    ui->sBHour->setStyleSheet(spdStyle::TimeField);
    ui->sBMin ->setStyleSheet(spdStyle::TimeField);
    ui->sBSec ->setStyleSheet(spdStyle::TimeField);

    ui->bRun->setEnabled(true);
    
    this->DeltaM=val;
    this->computeDeltaT();
    
    this->setLogtext(tr("-> Shift by ").toStdString()
                     +spdFunc::stoyear( this->computeDeltaT())
                     +" or "
                     +std::to_string(this->computeDeltaT())
                     +"s.\n");
}

void MainWIndow::on_sBHour_valueChanged(int val) {
    ui->sBYear->setStyleSheet(spdStyle::TimeField);
    ui->sBDay->setStyleSheet(spdStyle::TimeField);
    ui->sBHour->setStyleSheet(spdStyle::TimeField);
    ui->sBMin->setStyleSheet(spdStyle::TimeField);
    ui->sBSec->setStyleSheet(spdStyle::TimeField);

    ui->bRun->setEnabled(true);
    
    this->DeltaH=val;
    this->computeDeltaT();
    
    this->setLogtext(tr("-> Shift by ").toStdString()
                     +spdFunc::stoyear( this->computeDeltaT())
                     +" or "
                     +std::to_string(this->computeDeltaT())
                     +"s.\n");
}



void MainWIndow::on_rBInfo_clicked() { // About...
    this->qeBlur.setEnabled(true);

    int currentProgress=ui->progressBar->value();
    ui->progressBar->setValue(100);

    // Show some sys info...
    std::string sysInfo;
    sysInfo="<p><small><i>";
    sysInfo+=QSysInfo::machineHostName().toStdString()+"<br/>"
            + QSysInfo::prettyProductName().toStdString()+" "
            + QSysInfo::productVersion().toStdString()+" "
            +"- "+QSysInfo::kernelType().toStdString()+" "
            +QSysInfo::kernelVersion().toStdString()+" "
            +QSysInfo::currentCpuArchitecture().toStdString()+"<br/>"
            +"Build: "+"Qt "+std::string(qVersion())+ " - "
            +QSysInfo::buildCpuArchitecture().toStdString()+" "
            +"- ABI: "+QSysInfo::buildAbi().toStdString()
            +".";
    sysInfo+="</i></small></p>";

    QString infoMsg;    
    infoMsg=QString::fromStdString("<p><strong>shiftpicdate-gui</strong> "+std::string(VER)+"<br /><br />");
    infoMsg+=tr("Qt5 GUI for changing picture timestamps.</p>");
    infoMsg+=QString::fromStdString(sysInfo);
    infoMsg+="<p><a href='https://github.com/a-lemonnier/shiftpicdate-gui'>github.com/a-lemonnier/shiftpicdate-gui</a><br />";
    infoMsg+="MIT - A. Lemonnier - "+QDate::currentDate().toString("yyyy")+"</p>";

    QMessageBox msgBox;
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setTextInteractionFlags(Qt::TextBrowserInteraction);
    msgBox.setSizePolicy(QSizePolicy::Policy::Expanding,
                         QSizePolicy::Policy::Expanding);
    msgBox.setText(infoMsg);
    
    msgBox.exec();
    
    ui->progressBar->setValue(currentProgress);
    ui->rBInfo->setChecked(false);
    this->qeBlur.setEnabled(false);
}

void MainWIndow::on_bNext_clicked() {
    if (this->timer_ss->isActive()) timer_ss->stop();

    this->currentPic++; // Next pic
    if (this->currentPic>this->picNb-1) this->currentPic=0;

    QPixmap pmap(QString::fromStdString(this->vsList[this->currentPic]));

    ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(),ui->picLabel->size().width(),Qt::KeepAspectRatio));

    ui->picLabel->setToolTipDuration(1000);
    ui->picLabel->setToolTip(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));
    ui->statusbar->showMessage(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));
}

void MainWIndow::on_bPrev_clicked() {
    if (this->timer_ss->isActive()) timer_ss->stop();

    this->currentPic--;
    if (this->currentPic<0) this->currentPic=this->picNb-1;

    QPixmap pmap(QString::fromStdString(this->vsList[this->currentPic]));

    ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(),ui->picLabel->size().width(),Qt::KeepAspectRatio));

    ui->picLabel->setToolTipDuration(1000);
    ui->picLabel->setToolTip(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));
    ui->statusbar->showMessage(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));
}

void MainWIndow::on_bSelectfile_clicked() {
    qeBlur.setEnabled(true);

    this->timer_ss->stop();

    fsDialog* secWindow= new fsDialog(this);

    connect(secWindow, &fsDialog::sendvector, this, &MainWIndow::get_fsDialog_vector);//, Qt::BlockingQueuedConnection);
    connect(secWindow, &fsDialog::destroyed, this, &MainWIndow::deleteLater);
    connect(secWindow, &fsDialog::sendvector, this, [this]() { timer_ss->stop();
                                                               startSlideshow(); });
    secWindow->exec();

    qeBlur.setEnabled(false);
}

void MainWIndow::on_bFlag_released() {
  qApp->removeTranslator(&qTranslator);

  if (this->selectedLang==Lang::EN) {
    this->selectedLang=Lang::FR;
    if (qTranslator.load("french.qm"))
      qApp->installTranslator(&qTranslator);
    this->setLogtext(tr("- Switch language to French.\n").toStdString());
    ui->bFlag->setIcon(QIcon(":/flag/fr.svg"));
  }
  else {
    this->selectedLang=Lang::EN;
    this->setLogtext(tr("- Switch language to English.\n").toStdString());
    ui->bFlag->setIcon(QIcon(":/flag/eng.svg"));
  }
  ui->retranslateUi(this);
}

void MainWIndow::setLogtextTh(const std::string &msg) {
    this->SLog+=QString::fromStdString(msg); 
    if (this->isAutoScroll)
        ui->tBLog->verticalScrollBar()->setValue(ui->tBLog->verticalScrollBar()->maximum());
}

void MainWIndow::setLogtext(const std::string &msg) {
    this->SLog+=QString::fromStdString(msg);
    if (!this->isQuiet) {
        ui->tBLog->setText(this->SLog);
        if (this->isAutoScroll)
            ui->tBLog->verticalScrollBar()->setValue(ui->tBLog->verticalScrollBar()->maximum());
    }
}

void MainWIndow::setvsList(std::vector<std::string> &vsList) { this->vsList=vsList; }

std::vector<std::string> MainWIndow::getvsList() { return this->vsList; }

int  MainWIndow::computeDeltaT() { // compute total secs.
    constexpr int cM=60;
    constexpr int cH=cM*60;
    constexpr int cD=cH*24;
    constexpr int cY=cD*365;
    
    this->DeltaT=this->DeltaY*cY
    +this->DeltaD*cD
    +this->DeltaH*cH
    +this->DeltaM*cM
    +this->DeltaS;
    
    return this->DeltaT;
}

void MainWIndow::update_progressBar_value(int v) {
    int r=255-static_cast<float>(v)/100*255;
    int g=static_cast<float>(v)/100*255;
    // Color as function of v
    ui->progressBar->setStyleSheet(QString::fromStdString("color: rgb("+std::to_string(r)+", "+std::to_string(g)+", 0);"));
    ui->progressBar->setValue(v);
}

void MainWIndow::update_Log_value(QString str) { this->setLogtextTh(str.toUtf8().constData()); }

void MainWIndow::update_Log() {
    if (!this->isQuiet) {
        ui->tBLog->setText(this->SLog);
        if (this->isAutoScroll)
            ui->tBLog->verticalScrollBar()->setValue(ui->tBLog->verticalScrollBar()->maximum());
    }
}

void MainWIndow::getfileList() {
    fileList *fL=new fileList();
    QThread *th=new QThread();
    
    timer=new QTimer(nullptr);
    
    fL->moveToThread(th);
    fL->setfileName(this->sFilename);

    connect(timer, &QTimer::timeout, this, &MainWIndow::update_Log);
    
    // Update textBrowser at least one time
    connect(th, &QThread::finished, this, [=](){ this->timer->stop(); this->update_Log(); });
    
    connect(fL, &fileList::fLProgress, this,  &MainWIndow::update_progressBar_value);
    connect(fL, &fileList::sendstdStr, this,  &MainWIndow::update_Log_value);
    connect(fL, &fileList::finished, th, &QThread::quit);

    // Start slideshow when the thread is finished.
    connect(fL, &fileList::finished, this, [=](){
        this->vsList=fL->getvsList();
        this->startSlideshow();

        ui->bRun->setEnabled(true);
        ui->bReset->setEnabled(true);
        ui->bSelectfile->setEnabled(true);
        ui->bQuit->setEnabled(true);

        ui->sBYear->setStyleSheet(spdStyle::TimeFieldGreen);
        ui->sBDay->setStyleSheet(spdStyle::TimeFieldGreen);
        ui->sBHour->setStyleSheet(spdStyle::TimeFieldGreen);
        ui->sBMin->setStyleSheet(spdStyle::TimeFieldGreen);
        ui->sBSec->setStyleSheet(spdStyle::TimeFieldGreen);

        ui->bRun->setStyleSheet(spdStyle::bRunGreen);
        ui->tBLog->setStyleSheet(spdStyle::tbLogGreen);
    }, Qt::BlockingQueuedConnection);

    connect(th, &QThread::finished, th, &QThread::deleteLater);
    connect(th, &QThread::finished, fL, &fileList::deleteLater);
    connect(th, &QThread::started, fL, &fileList::getList);

    this->setLogtextTh(tr("- List of files...\n").toStdString());
    
    timer->start(1000);
    th->start();
}

void MainWIndow::startSlideshow() {
    if (!this->vsList.empty()) {
        ui->bRot->setEnabled(true);
        ui->bNext->setEnabled(true);
        ui->bPrev->setEnabled(true);
        ui->bStop->setEnabled(true);

        this->picNb=vsList.size();
        this->currentPic=0;

        // Show the first img
        QPixmap pmap(QString::fromStdString(this->vsList[0]));
        ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(), ui->picLabel->size().width(),Qt::KeepAspectRatio));
        // --

        // Timer --
        timer_ss=new QTimer(this);
        connect(timer_ss, &QTimer::timeout, this, &MainWIndow::changePic);
        connect(timer_ss, &QTimer::destroyed, timer_ss, &QTimer::deleteLater);
//        connect(ui->bSelectfile, &QPushButton::clicked, timer_ss, &QTimer::stop);
        timer_ss->start(this->iSlideshowInterval);
        // --
    }
    else std::cerr << "- MainWIndow::startSlideshow(): empty list.";
}

void MainWIndow::changePic() {
    this->currentPic++;
    if (this->currentPic>this->picNb-1) this->currentPic=0;

    QPixmap pmap(QString::fromStdString(this->vsList[this->currentPic]));
    
    QTransform transform(QTransform().rotate(this->iPicRot));
    pmap = pmap.transformed(transform);

    ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(),ui->picLabel->size().width(),Qt::KeepAspectRatio));

    ui->picLabel->setToolTipDuration(1000);
    ui->picLabel->setToolTip(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));
    ui->statusbar->showMessage(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));

    this->iPicRot=0;
}

void MainWIndow::get_fsDialog_vector(std::vector<std::string> &vs) {
    this->setLogtext("- "+std::to_string(abs(static_cast<long>(vs.size()-this->vsList.size())))+tr(" files removed.").toStdString());
    this->vsList=vs;
}

void MainWIndow::run_shift() {
  runShift *rs=new runShift();
  QThread *th=new QThread();

  timer=new QTimer(nullptr);

  rs->moveToThread(th);
  rs->setvsList(this->vsList);
  rs->setDiff(this->DeltaT);
  
  connect(timer, &QTimer::timeout, this, &MainWIndow::update_Log);

  connect(th, &QThread::finished, this, [=](){ this->timer->stop(); this->update_Log(); });

  connect(rs, &runShift::sendProgress, this,  &MainWIndow::update_progressBar_value);
  connect(rs, &runShift::sendstdStr, this,  &MainWIndow::update_Log_value);
  connect(rs, &runShift::finished, th, &QThread::quit);

  connect(th, &QThread::finished, th, &QThread::deleteLater);
  connect(th, &QThread::finished, rs, &runShift::deleteLater);
  connect(th, &QThread::started, rs, &runShift::shift);

  timer->start(1000);
  th->start();
}


void MainWIndow::plotHist() {
/*
1. Get a vector of epochs.
2. Determine if the files spread over one years, or one month or one day.
3.
If they spread over 12m then fill 12 bins that are numbered: "1", "2", "..." or by month if there the place to put it.
If they spread over 1m then fill 4 bins of 7 days.
And so on until 24h.

4. Fill the sets and axis ticks.
5. Plot after GetList and SelectFiles.

*/

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_real_distribution<double> distrib(0.5,1);

    for(int i=0;i<10;i++) {
       QtCharts::QBarSet *set=new QtCharts::QBarSet(QString::fromStdString("data_"+std::to_string(i)));
       for(int j=0;j<10;j++)
           *set << distrib(gen);
       bSerie->append(set);
    }

    this->Chart->addSeries(this->bSerie);

    this->Chart->legend()->setVisible(false);
}


void MainWIndow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange)
      ui->retranslateUi(this);
    else QWidget::changeEvent(event);
}


// fileList:: ------------
// -----------------------

void fileList::getList() {
    int iCount=0;
    size_t iFileNb=spdFunc::fileNb(this->fileName);

    emit(fLProgress(0)); // Set progressBar to 0
    emit(sendstdStr(QString::fromStdString(tr("\t - Number of files: ").toStdString()+std::to_string(iFileNb)+" -\n")));

    this->vsList.clear();
    for(const auto &str: fs::recursive_directory_iterator(this->fileName)) {
        if (spdFunc::test_ext(str.path().filename().string())) {
            this->vsList.emplace_back(str.path().string());
            auto [sTmp, Epoch]=spdFunc::getExifDateEpoch(str.path().string());
            sTmp+=" "+std::to_string(Epoch)+" s";
            if (sTmp.empty())
                emit(sendstdStr(QString::fromStdString("\t"+str.path().string()+"\n")));
            else
                emit(sendstdStr(QString::fromStdString("\t"+str.path().string()+"\t"+sTmp+"\n")));

         }
        emit(fLProgress(static_cast<float>((iCount++)*100/iFileNb))); // Set progressBar to n %
    }
    std::sort(this->vsList.begin(), this->vsList.end());

    emit(fLProgress(100)); // Set progressBar to 100
    emit(finished());
}

void fileList::setfileName(std::string& str) { this->fileName=str; }

std::vector<std::string> fileList::getvsList() const { return this->vsList; }


// runShift:: ----------
// ---------------------

void runShift::setvsList(std::vector<std::string> &vsList) { this->vsList=vsList; }

void runShift::shift() {
    emit(sendProgress(0));
    emit(sendstdStr(tr("- Start shifting pictures.\n")));

    int iCount=0, iFileNb=this->vsList.size();
    for(const auto &file: this->vsList) {
        std::string sTmp(spdFunc::getExifDate(file));
        if (!sTmp.empty()) {
            spdFunc::setExifDate(file, this->Diff, this->bIsDST);
            emit(sendstdStr(QString::fromStdString("\t"+file+"\t"+sTmp+".\n")));
        }
        emit(sendProgress(static_cast<float>((iCount++)*100/iFileNb)));
    }
    emit(sendProgress(100));
    emit(sendstdStr(tr("- Shifting completed.\n")));
    emit(finished());
}

void runShift::setDiff(long t) { this->Diff= (t>0) ? t : 0; }

void runShift::setDST(bool bIsDST) { this->bIsDST=bIsDST; }
