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
, iSlideshowInterval(1500)
, currentPic(0)
, lHistCurYear(0)
, curHistTheme(Theme::DARK)
, selectedLang(Lang::EN)
, hasUpdate(false)
, autoWrap (true)
, isPicHidden(false)
{
    ui->setupUi(this);
    this->setWindowTitle(QString::fromStdString("shiftpicdate-gui "+
                                                std::to_string(VER_MAJOR)+"."+
                                                std::to_string(VER_MINOR)+"."+
                                                std::to_string(VER_REV)));

    // Set translation --
    QString defaultLocale = QLocale::system().name();

    ui->bFlag->setIcon(QIcon(":/flag/eng.svg"));

    if (defaultLocale.toStdString().find("fr")!=std::string::npos ||
        defaultLocale.toStdString().find("FR")!=std::string::npos ||
        defaultLocale.toStdString().find("Fr")!=std::string::npos) {
        this->selectedLang=Lang::FR;
        if (qTranslator.load("french.qm"))
          qApp->installTranslator(&qTranslator);
        this->setLogtext(tr("- Switch language to French.").toStdString());
        ui->bFlag->setIcon(QIcon(":/flag/fr.svg"));
        ui->retranslateUi(this);
    }

    // ------------------

    // Icon --
    this->setWindowIcon(QIcon(":/ico/shiftpicdate-gui.ico"));

    // - + ▢ ↻ ◁ ▷
    QSize iconSizeR(16,16);
    QSize iconSize(32,32);

    ui->bScroll->text().clear();
    ui->bScroll->setIconSize(iconSize);
    ui->bScroll->setIcon(QIcon(":/ico/auto_scroll_on.svg"));

    ui->bWrap->text().clear();
    ui->bWrap->setIconSize(iconSize);
    ui->bWrap->setIcon(QIcon(":/ico/wrap_lines_on.svg"));

    ui->bQuiet->text().clear();
    ui->bQuiet->setIconSize(iconSize);
    ui->bQuiet->setIcon(QIcon(":/ico/verbose.svg"));

    ui->bZoomOut->text().clear();
    ui->bZoomOut->setIconSize(iconSizeR);
    ui->bZoomOut->setIcon(QIcon(":/ico/minus.svg"));

    ui->bZoomIn->text().clear();
    ui->bZoomIn->setIconSize(iconSizeR);
    ui->bZoomIn->setIcon(QIcon(":/ico/plus.svg"));

    ui->bHidepic->text().clear();
    ui->bHidepic->setIconSize(iconSize);
    ui->bHidepic->setIcon(QIcon(":/ico/eye_on.svg"));

    ui->bRot->text().clear();
    ui->bRot->setIconSize(iconSize);
    ui->bRot->setIcon(QIcon(":/ico/rot_bw.svg"));

    ui->bStop->text().clear();
    ui->bStop->setIconSize(iconSize);
    ui->bStop->setIcon(QIcon(":/ico/stop_bw.svg"));

    ui->bNext->text().clear();
    ui->bNext->setIconSize(iconSize);
    ui->bNext->setIcon(QIcon(":/ico/next_bw.svg"));

    ui->bPrev->text().clear();
    ui->bPrev->setIconSize(iconSize);
    ui->bPrev->setIcon(QIcon(":/ico/prev_bw.svg"));
    // -------


    // Version --
    qApp->setApplicationName(APPNAME);
    qApp->setApplicationVersion(QString::fromStdString(std::to_string(VER_MAJOR) + "." +
                                                       std::to_string(VER_MINOR) + "." +
                                                       std::to_string(VER_REV)));
    // ----------

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

    ui->cbYear->setHidden(true);
    ui->SSprogressBar->setHidden(true);
    // -----------------

    // Set values --
    ui->bDST->setChecked(this->isDST);
    
    ui->sBYear->setValue(this->DeltaY);
    ui->sBDay->setValue(this->DeltaD);
    ui->sBHour->setValue(this->DeltaH);
    ui->sBMin->setValue(this->DeltaM);
    ui->sBSec->setValue(this->DeltaS);

    ui->rBInfo->setFocus();
    // -------------

    // Set Styles --
    ui->lEPath->setStyleSheet(spdStyle::lEPathRed);
    ui->bBrowse->setStyleSheet(spdStyle::bBrowseRed);
    ui->bBrowseFile->setStyleSheet(spdStyle::bBrowseRed);
    ui->tBLog->setAcceptRichText(true);
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
    ui->cbYear->setDuplicatesEnabled(false);

    this->Chart=new QtCharts::QChart();
    this->chartView = new QtCharts::QChartView(this->Chart);

    this->Chart->setAnimationOptions(QtCharts::QChart::AllAnimations);
    this->Chart->setTheme(QtCharts::QChart::ChartThemeBlueIcy);

    this->Chart->setToolTip(tr("Time line."));

    this->chartView->setRenderHint(QPainter::Antialiasing);
    this->chartView->setMaximumHeight(200);

    ui->hlBarChart->setSizeConstraint(QLayout::SetFixedSize);
    ui->hlBarChart->addWidget(this->chartView);

    this->chartView->setHidden(true);
    // --------

    // Event --
    setMouseTracking(true);
    // --------

    // Check update --
    this->checkForUpdate();
    // ---------------

    // Define some connections --
    connect(ui->tBLog->horizontalScrollBar(), &QScrollBar::sliderMoved, [this]() {ui->tBLog->setStyleSheet("");});
    connect(ui->tBLog->verticalScrollBar(),   &QScrollBar::sliderMoved, [this]() {ui->tBLog->setStyleSheet("");});

    // Blink
    connect(ui->bReset, &QPushButton::clicked, this, &MainWIndow::getButtontoBlink);
    connect(ui->bRun, &QPushButton::clicked, this, &MainWIndow::getButtontoBlink);
    connect(ui->bFlag, &QPushButton::clicked, this, &MainWIndow::getButtontoBlink);
    connect(ui->bTest, &QPushButton::clicked, this, &MainWIndow::getButtontoBlink);

    QTimer::singleShot(1500, this, [this]() {this->blinkButton(ui->bFlag);});
    // -----------------------
}

MainWIndow::~MainWIndow() { delete ui; }

void MainWIndow::on_bBrowse_clicked() {
    qeBlur.setEnabled(true);
    QFileDialog dialog(this);

// Gnome issue
#if defined(__linux__)
    QProcess process;
    process.start("pgrep", QStringList("gnome-session"));
    process.waitForReadyRead();
    if(!process.readAllStandardOutput().isEmpty())
        dialog.setFileMode(QFileDialog::DirectoryOnly);
    else
        dialog.setOption(QFileDialog::ShowDirsOnly, true);
#else
    dialog.setFileMode(QFileDialog::DirectoryOnly);
#endif

//    // See on QT forum ***
//    QListView *list = dialog.findChild<QListView*>("listView");
//    if (list)  list->setSelectionMode(QAbstractItemView::MultiSelection);
//    QTreeView *tree = dialog.findChild<QTreeView*>();
//    if (tree)  tree->setSelectionMode(QAbstractItemView::MultiSelection);
//    // ***
    
    QStringList fileNames;
    QStringList fileNames_tmp;

    if (dialog.exec()) fileNames_tmp=dialog.selectedFiles();

    if (fileNames_tmp.empty()) {
        fileNames.append("");
        this->setLogtext(tr("- Path selected: empty path.").toStdString()+"\n");
    }
    else {
        this->vsList.clear();

        fileNames=fileNames_tmp;
        fileNames_tmp.clear();
        this->setLogtext(tr("- Path selected: ").toStdString());
        for(const auto &str: fileNames) this->setLogtext(str.toUtf8().toStdString()+"\n");

        ui->lEPath->setText(fileNames[0]);
        ui->lEPath->setEnabled(false);

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
                    this->changeTaskbarOverlay(":/ico/play.svg");
                    this->setEnabled(false);
                    this->getfileList();
                    this->setEnabled(true);
                }
                ui->lEPath->setStyleSheet("");
                ui->bBrowse->setStyleSheet("");
                ui->bBrowseFile->setStyleSheet("");
            }
            else
                this->setLogtext(tr("- Cannot process with this path: ").toStdString()+this->sFilename+"!\n");
        }
    }
    qeBlur.setEnabled(false);
}

void MainWIndow::on_bBrowseFile_clicked() {
    qeBlur.setEnabled(true);
    QFileDialog dialog(this);

    dialog.setViewMode(QFileDialog::List);

    QStringList fileNames;
    QStringList fileNames_tmp;

    if (dialog.exec()) fileNames_tmp=dialog.selectedFiles();

    if (fileNames_tmp.empty()) {
        fileNames.append("");
        this->setLogtext(tr("- Path selected: empty path.").toStdString()+"\n");
    }
    else {
        this->vsList.clear();

        fileNames=fileNames_tmp;
        fileNames_tmp.clear();
        this->setLogtext(tr("- Path selected: ").toStdString());
        for(const auto &str: fileNames) this->setLogtext(str.toUtf8().toStdString()+"\n");

        ui->lEPath->setText(fileNames[0]);
        ui->lEPath->setEnabled(false);

        if (fs::is_regular_file(fileNames[0].toUtf8().toStdString())) {
            this->sFilename=fileNames[0].toUtf8().toStdString();
            this->setLogtext(tr("- Continue with one file").toStdString()+": "
                            +spdFunc::getExifDate(this->sFilename)+".\n");

            ui->bReset->setEnabled(true);

            QImageReader* imgr=new QImageReader(QString::fromStdString(this->sFilename));
            imgr->read();

            if (imgr->error())
                std::cerr << tr("- Error: ").toStdString()
                          << imgr->errorString().toStdString()
                          << " ( code " << imgr->error() <<" )\n";

            // Rescale pictures
            QPixmap pmap(QString::fromStdString(this->sFilename));
            ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(),ui->picLabel->size().width(),Qt::KeepAspectRatio));

            delete imgr;

            this->setEnabled(false);
            this->vsList.push_back(this->sFilename);
            this->setEnabled(true);

            ui->lEPath->setStyleSheet("");
            ui->bBrowse->setStyleSheet("");
            ui->bBrowseFile->setStyleSheet("");

            ui->bRun->setEnabled(true);
            ui->bReset->setEnabled(true);
            ui->bQuit->setEnabled(true);

            ui->sBYear->setStyleSheet(spdStyle::TimeFieldGreen);
            ui->sBDay->setStyleSheet(spdStyle::TimeFieldGreen);
            ui->sBHour->setStyleSheet(spdStyle::TimeFieldGreen);
            ui->sBMin->setStyleSheet(spdStyle::TimeFieldGreen);
            ui->sBSec->setStyleSheet(spdStyle::TimeFieldGreen);

            ui->bRun->setStyleSheet(spdStyle::bRunGreen);
            ui->tBLog->setStyleSheet(spdStyle::tbLogGreen);
        }
        else
            this->setLogtext(tr("- Cannot process with this path: ").toStdString()+this->sFilename+"!\n");
    }
    qeBlur.setEnabled(false);
}

void MainWIndow::on_bRun_clicked() {
    if (this->timer_ss->isActive()) this->timer_ss->stop();

    ui->sBYear->setStyleSheet(spdStyle::TimeField);
    ui->sBDay->setStyleSheet(spdStyle::TimeField);
    ui->sBHour->setStyleSheet(spdStyle::TimeField);
    ui->sBMin->setStyleSheet(spdStyle::TimeField);
    ui->sBSec->setStyleSheet(spdStyle::TimeField);

    ui->bRun->setEnabled(false);
    ui->bQuit->setEnabled(true);
    ui->bBrowse->setEnabled(false);
    ui->bBrowseFile->setEnabled(false);
    ui->bReset->setEnabled(false);
    ui->sBYear->setEnabled(false);
    ui->sBDay->setEnabled(false);
    ui->sBHour->setEnabled(false);
    ui->sBMin->setEnabled(false);
    ui->sBSec->setEnabled(false);

    this->changeTaskbarOverlay(":/ico/play.svg");

    if (this->vsList.size()>1) this->chartView->setHidden(true);

    this->run_shift();

    if (this->vsList.size()>1) this->chartView->setHidden(false);

    ui->bRun->setEnabled(true);
    ui->bQuit->setEnabled(true);
    ui->bBrowse->setEnabled(true);
    ui->bBrowseFile->setEnabled(true);
    ui->bReset->setEnabled(true);
    ui->sBYear->setEnabled(true);
    ui->sBDay->setEnabled(true);
    ui->sBHour->setEnabled(true);
    ui->sBMin->setEnabled(true);
    ui->sBSec->setEnabled(true);

    ui->tBLog->setStyleSheet("");
    ui->bRun->setStyleSheet("");
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
void MainWIndow::on_bTest_clicked() { ; }
void MainWIndow::on_bStop_clicked() { if (this->timer_ss->isActive()) timer_ss->stop(); }

void MainWIndow::on_bHidepic_clicked() {
    this->isPicHidden=!this->isPicHidden;
    if (this->isPicHidden) ui->bHidepic->setIcon(QIcon(":/ico/eye_off.svg"));
    else ui->bHidepic->setIcon(QIcon(":/ico/eye_on.svg"));

    ui->bStop->setHidden(this->isPicHidden);
    ui->bRot->setHidden(this->isPicHidden);
    ui->bPrev->setHidden(this->isPicHidden);
    ui->bNext->setHidden(this->isPicHidden);
    ui->picLabel->setHidden(this->isPicHidden);
    ui->SSprogressBar->setHidden(this->isPicHidden);
}

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


void MainWIndow::on_bWrap_clicked() {
    this->autoWrap=!this->autoWrap;
    if (!this->autoWrap) {
        ui->bWrap->setIcon(QIcon(":/ico/wrap_lines_off.svg"));
        ui->tBLog->setLineWrapMode(QTextEdit::NoWrap);
        this->setLogtext(tr("- Auto wrap off.\n").toStdString());
    }
    else {
        ui->bWrap->setIcon(QIcon(":/ico/wrap_lines_on.svg"));
        ui->tBLog->setLineWrapMode(QTextEdit::WidgetWidth);
        this->setLogtext(tr("- Auto wrap on.\n").toStdString());
    }
}

void MainWIndow::on_bQuiet_clicked() {
    this->isQuiet=!this->isQuiet;
    if (this->isQuiet) ui->bQuiet->setIcon(QIcon(":/ico/quiet.svg"));
    else ui->bQuiet->setIcon(QIcon(":/ico/verbose.svg"));

    if (!this->isQuiet) this->setLogtext(tr("- Toggle verbosity on.\n").toStdString());
}

void MainWIndow::on_bScroll_clicked() {
    this->isAutoScroll=!this->isAutoScroll;

    if (this->isAutoScroll) ui->bScroll->setIcon(QIcon(":/ico/auto_scroll_on.svg"));
    else ui->bScroll->setIcon(QIcon(":/ico/auto_scroll_off.svg"));

    if (!this->isQuiet)
        if (this->isAutoScroll) this->setLogtext(tr("- Toggle auto scroll on.\n").toStdString());
        else         this->setLogtext(tr("- Toggle auto scroll off.\n").toStdString());
}

void MainWIndow::on_bReset_clicked() {
    if (this->timer_ss->isActive()) this->timer_ss->stop();

    ui->bRun->setEnabled(true);
    ui->sBYear->setEnabled(true);
    ui->sBDay->setEnabled(true);
    ui->bDST->setEnabled(true);
    ui->lEPath->setEnabled(true);
    
    ui->bDST->setChecked(this->isDST);

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

    this->blinkRButton(ui->rBInfo);

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
    infoMsg=QString::fromStdString("<p><strong>shiftpicdate-gui</strong> "+
                                   std::to_string(VER_MAJOR)+"."+
                                   std::to_string(VER_MINOR)+"."+
                                   std::to_string(VER_REV)
                                   +"<br /><br />");

    infoMsg+=tr("Qt5 GUI for changing picture timestamps.</p>");

    if (this->hasUpdate)
        infoMsg+="<p><i>"+tr("Update available")+": "
                +"<a href='https://sourceforge.net/projects/shiftpicdate-gui'>"
                +QString::fromStdString(this->lastRelease)
                +"</a>.</p>";

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

    this->changeTaskbarPic(pmap);

    ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(),ui->picLabel->size().width(),Qt::KeepAspectRatio));

    ui->picLabel->setToolTipDuration(1000);
    ui->picLabel->setToolTip(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));
    ui->statusbar->showMessage(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));

    ui->SSprogressBar->setRange(1, this->picNb);
    ui->SSprogressBar->setValue(this->currentPic+1);
}

void MainWIndow::on_bPrev_clicked() {
    if (this->timer_ss->isActive()) timer_ss->stop();

    this->currentPic--;
    if (this->currentPic<0) this->currentPic=this->picNb-1;

    QPixmap pmap(QString::fromStdString(this->vsList[this->currentPic]));

    this->changeTaskbarPic(pmap);

    ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(),ui->picLabel->size().width(),Qt::KeepAspectRatio));

    ui->picLabel->setToolTipDuration(1000);
    ui->picLabel->setToolTip(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));
    ui->statusbar->showMessage(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));

    ui->SSprogressBar->setRange(1, this->picNb);
    ui->SSprogressBar->setValue(this->currentPic+1);
}

void MainWIndow::on_bSelectfile_clicked() {
    qeBlur.setEnabled(true);

    this->changeTaskbarOverlay(":/ico/gears_1.svg");

    this->timer_ss->stop();

    fsDialog* secWindow= new fsDialog(this);

    connect(secWindow, &fsDialog::sendvector, this, &MainWIndow::get_fsDialog_vector);//, Qt::BlockingQueuedConnection);
    connect(secWindow, &fsDialog::destroyed, this, &MainWIndow::deleteLater);
    connect(secWindow, &fsDialog::sendvector, this, [this]() { timer_ss->stop();
                                                               startSlideshow(); });

    connect(secWindow, &fsDialog::rejected, this, [this]() {qeBlur.setEnabled(false);});

    connect(secWindow, &fsDialog::finished, this,
            [this]() {
                this->changeTaskbarOverlay(":/ico/pause.svg");

                this->lHistCurYear=0;
                this->vEpoch.clear();
                for(const auto &file: this->vsList)
                    this->vEpoch.emplace_back(spdFunc::getExifEpoch(file));
                this->plotHist();
    });

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

void MainWIndow::on_cbYear_activated(const QString &str) {
    if (!str.isEmpty())
        this->lHistCurYear=std::stol(str.toStdString());
    this->plotHist();
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

int MainWIndow::computeDeltaT() { // compute total secs.
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

void MainWIndow::changeTaskbarOverlay(const QString &str) {
#if defined(_WIN32) || defined(WIN32)
    this->tbarButton->clearOverlayIcon();
    this->tbarButton->setOverlayIcon(QIcon(str));
#endif
}

void MainWIndow::changeTaskbarPic(const QPixmap &pm) {
#if defined(_WIN32) || defined(WIN32)
    this->tbarThumb->setIconicLivePreviewPixmap(pm);
    this->tbarThumb->setIconicThumbnailPixmap(pm);
#endif
}

void MainWIndow::blinkButton(QPushButton *button) {
    button->setFocus();

    int Blink_nb=BLINK_NB;
#if defined(_WIN32) || defined(WIN32)
    int Blink_freq=75;
#else
    int Blink_freq=40;
#endif
    int Blink_dur=Blink_freq*Blink_nb;

    QTimer *timer_blink=new QTimer();

    QElapsedTimer elapsedT;
    elapsedT.start();

    connect(timer_blink, &QTimer::timeout, this, [elapsedT, Blink_freq, Blink_dur, button,timer_blink](){
            if (elapsedT.elapsed()+Blink_freq>=Blink_dur) {
#if defined(_WIN32) || defined(WIN32)
                button->setStyleSheet("");
#else
                button->setFlat(false);
#endif
                button->clearFocus();
                timer_blink->stop();
            }
            else {
#if defined(_WIN32) || defined(WIN32)
                if (button->styleSheet().isEmpty())
                  button->setStyleSheet(spdStyle::Blink);
                else
                  button->setStyleSheet("");
#else
                  button->setFlat(!button->isFlat());
#endif
            }
    });
    connect(timer_blink, &QTimer::destroyed, timer_blink, &QTimer::deleteLater);
    timer_blink->start(Blink_freq);
}

void MainWIndow::blinkRButton(QRadioButton *rbutton) {
    int Blink_nb=BLINK_NB;
    int Blink_freq=100;
    int Blink_dur=Blink_freq*Blink_nb;

    QTimer *timer_blink=new QTimer();

    QElapsedTimer elapsedT;
    elapsedT.start();

    connect(timer_blink, &QTimer::timeout, this, [this, elapsedT, Blink_freq, Blink_dur, rbutton, timer_blink](){
            if (elapsedT.elapsed()+Blink_freq>=Blink_dur) {
                rbutton->setChecked(false);
                rbutton->clearFocus();
                timer_blink->stop();
            }
            else
                rbutton->setChecked(!rbutton->isChecked());
    });
    connect(timer_blink, &QTimer::destroyed, timer_blink, &QTimer::deleteLater);
    timer_blink->start(Blink_freq);
}

void MainWIndow::update_progressBar_value(int v) {
    int r=255-static_cast<float>(v)/100*255;
    int g=static_cast<float>(v)/100*255;
    // Color as function of v
    ui->progressBar->setStyleSheet(QString::fromStdString("color: rgb("+std::to_string(r)+", "+std::to_string(g)+", 0);"));
    ui->progressBar->setValue(v);

#if defined(_WIN32) || defined(WIN32)
    this->tbarProgress->setValue(v);
#endif
}

void MainWIndow::update_SSprogressBar_value(int v) { ui->SSprogressBar->setValue(v); }

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

    this->vEpoch.clear();

    connect(timer, &QTimer::timeout, this, &MainWIndow::update_Log);
    
    // Update textBrowser at least one time
    connect(th, &QThread::finished, this, [=](){ this->timer->stop(); this->update_Log(); });
    
    connect(fL, &fileList::fLProgress, this,  &MainWIndow::update_progressBar_value);
    connect(fL, &fileList::sendstdStr, this,  &MainWIndow::update_Log_value);
    connect(fL, &fileList::sendEpoch, this, &MainWIndow::addEpoch);

    connect(fL, &fileList::finished, th, &QThread::quit);
    connect(fL, &fileList::finished, this, &MainWIndow::plotHist);

    // Start slideshow/plot when the thread is finished.
    connect(fL, &fileList::finished, this, [=](){
            this->vsList=fL->getvsList();
            if (!this->vsList.empty()) {

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

                this->changeTaskbarOverlay(":/ico/pause.svg");
        }
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

        ui->SSprogressBar->setHidden(false);

        this->picNb=static_cast<long>(vsList.size());
        this->currentPic=0;

        ui->SSprogressBar->setValue(1);
        ui->SSprogressBar->setRange(1, this->picNb);

        // Show the first img
        QPixmap pmap(QString::fromStdString(this->vsList[0]));
        ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(), ui->picLabel->size().width(),Qt::KeepAspectRatio));
        // --

        // Timer --
        timer_ss=new QTimer(this);
        connect(timer_ss, &QTimer::timeout, this, &MainWIndow::changePic);
        connect(timer_ss, &QTimer::destroyed, timer_ss, &QTimer::deleteLater);
        timer_ss->start(this->iSlideshowInterval);
        // --
    }
    else std::cerr << "- MainWIndow::startSlideshow(): empty list.";
}


void MainWIndow::initTaskBar() {
#if defined(_WIN32) || defined(WIN32)
      this->tbarButton = new QWinTaskbarButton();
      this->tbarButton->setWindow(this->windowHandle());
      this->tbarButton->setOverlayIcon(QIcon(":/ico/pause.svg"));

      this->tbarProgress = this->tbarButton->progress();
      this->tbarProgress->setVisible(true);

      this->tbarProgress->setRange(0,100);

      tbarThumb = new QWinThumbnailToolBar();
      this->tbarThumb->setWindow(this->windowHandle());
#endif
}

void MainWIndow::checkForUpdate(){
    this->setLogtext(tr("- Check update").toStdString()+".\n");
    QNetworkRequest request;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::SecureProtocols);
    request.setSslConfiguration(config);

    request.setUrl(QUrl("https://sourceforge.net/projects/shiftpicdate-gui/best_release.json"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    manager->get(request);

    connect(manager, &QNetworkAccessManager::finished, this, &MainWIndow::replyFinished);
}

void MainWIndow::changePic() {
    this->currentPic++;
    if (this->currentPic>this->picNb-1) this->currentPic=0;

    QPixmap pmap(QString::fromStdString(this->vsList[this->currentPic]));
    
    this->changeTaskbarPic(pmap);

    QTransform transform(QTransform().rotate(this->iPicRot));
    pmap = pmap.transformed(transform);

    ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(),ui->picLabel->size().width(),Qt::KeepAspectRatio));

    ui->picLabel->setToolTipDuration(1000);
    ui->picLabel->setToolTip(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));
    ui->statusbar->showMessage(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));

    this->update_SSprogressBar_value(this->currentPic+1);

    this->iPicRot=0;
}

void MainWIndow::get_fsDialog_vector(std::vector<std::string> &vs) {
    this->setLogtext("- "+std::to_string(abs(static_cast<long>(vs.size()-this->vsList.size())))+tr(" files removed.").toStdString()+"\n");
    this->vsList=vs;
}

void MainWIndow::addEpoch(long t) { this->vEpoch.push_back(t); }

void MainWIndow::replyFinished(QNetworkReply* reply) {
    QString data = (QString) reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data.toUtf8());

    if (reply->error()==QNetworkReply::NoError) {
        
        QJsonObject jsonObject = jsonDoc.object();

#if defined(_WIN32) || defined(WIN32)
        std::string sRelease=jsonObject.value("platform_releases").toObject().value("windows").toObject().value("filename").toString().toStdString();
#else
        std::string sRelease=jsonObject.value("platform_releases").toObject().value("linux").toObject().value("filename").toString().toStdString();
#endif

        if (sRelease.find(".tar.gz")!=std::string::npos)
            sRelease.erase(sRelease.find(".tar.gz"), std::string(".tar.gz").size());
        if (sRelease.find(".zip")!=std::string::npos)
            sRelease.erase(sRelease.find(".zip"), std::string(".zip").size());
        if (sRelease.find(".exe")!=std::string::npos)
            sRelease.erase(sRelease.find(".exe"), std::string(".exe").size());

        sRelease=std::regex_replace(sRelease, std::regex(R"([^\d.])"), "");
        sRelease.erase(sRelease.size(),1);

        int dot1, dot2;
        dot1=sRelease.find_first_of(".");
        dot2=sRelease.find_last_of(".");
        
        sRelease.erase(std::remove(sRelease.begin(), sRelease.end(), '.'), sRelease.end());
        
        int major_ver=0, minor_ver=0, rev_ver=0;
        
        try {
            if (dot1<dot2) {
                major_ver=std::stoi(sRelease.substr(0, dot1));
                minor_ver=std::stoi(sRelease.substr(dot1, dot2-dot1-1));
                rev_ver=std::stoi(sRelease.substr(dot2-1, sRelease.size()-dot2+1));
            }
            else {
                major_ver=std::stoi(sRelease.substr(0, dot1));
                minor_ver=std::stoi(sRelease.substr(dot1, sRelease.size()-dot1));
            }
        }
        catch (const std::exception& e) {
                std::cerr << "- Error while parsing version string: " << e.what() << ".\n";
        }
        
        if (major_ver>VER_MAJOR) this->hasUpdate=true;
        else if (major_ver==VER_MAJOR) {
             if (minor_ver>VER_MINOR) this->hasUpdate=true;
            else if (minor_ver==VER_MINOR) {
                 if (rev_ver>VER_REV) this->hasUpdate=true;
                 else this->hasUpdate=false;
            }
            else this->hasUpdate=false;
        }
        else this->hasUpdate=false;
        
        if (this->hasUpdate) {
            this->setLogtext(tr("- New release: ").toStdString()+
                            std::to_string(major_ver) + "." +
                            std::to_string(minor_ver) + "." +
                            std::to_string(rev_ver)+
                            ".\n");
            this->lastRelease=std::to_string(major_ver) + "." +
                              std::to_string(minor_ver) + "." +
                              std::to_string(rev_ver);
        }
        std::cout << "Distant version: " << std::to_string(major_ver) + "." +
                                            std::to_string(minor_ver) + "." +
                                            std::to_string(rev_ver) << "\n";
        std::cout << "Local version " <<    std::to_string(VER_MAJOR) + "." +
                                            std::to_string(VER_MINOR) + "." +
                                            std::to_string(VER_REV) << "\n";
    }
    else {
        this->hasUpdate=false;
        this->lastRelease=std::to_string(VER_MAJOR) + "." +
                          std::to_string(VER_MINOR) + "." +
                          std::to_string(VER_REV);
    }
}

void MainWIndow::getButtontoBlink() {
    this->blinkButton(dynamic_cast<QPushButton*>(sender()));
}

void MainWIndow::getRButtontoBlink() {
    this->blinkRButton(dynamic_cast<QRadioButton*>(sender()));
}

void MainWIndow::run_shift() {
  runShift *rs=new runShift();
  QThread *th=new QThread();

  timer=new QTimer(nullptr);

  rs->moveToThread(th);
  rs->setvsList(this->vsList);
  rs->setDiff(this->DeltaT);
  rs->setDST(this->isDST);
  
  connect(timer, &QTimer::timeout, this, &MainWIndow::update_Log);

  connect(rs, &runShift::sendProgress, this,  &MainWIndow::update_progressBar_value);
  connect(rs, &runShift::sendstdStr, this,  &MainWIndow::update_Log_value);
  connect(rs, &runShift::finished, th, &QThread::quit);

  connect(rs, &runShift::finished,  this, [this]() {

        this->timer->stop();
        this->update_Log();

        this->changeTaskbarOverlay(":/ico/pause.svg");

        if (this->vsList.size()>1) {
            this->lHistCurYear=0;
            ui->cbYear->clear();
            ui->cbYear->clearFocus();
            ui->cbYear->addItem(" ");
            ui->cbYear->setCurrentIndex(0);

            this->getfileList();

            this->Chart->removeAllSeries();
            this->plotHist();
      }
  });

  connect(th, &QThread::finished, th, &QThread::deleteLater);
  connect(th, &QThread::finished, rs, &runShift::deleteLater);
  connect(th, &QThread::started, rs, &runShift::shift);

  timer->start(1000);
  th->start();
}

void MainWIndow::plotHist() {
    using namespace QtCharts;

    if (!this->vEpoch.empty()) {
        typedef std::tuple<long, long, long, long, long, long> ymdHMS;

        std::vector<ymdHMS> vtEpoch;

        struct PicStat {
            long Y;
            long M;
            long D;
            long n;
        };

        struct PicDay {
            long Y;
            long M;
            long D;
        };

        std::vector<PicStat> vStat;
        std::vector<PicDay> vDay;

        std::sort(this->vEpoch.begin(), this->vEpoch.end());

        for(const auto &t: this->vEpoch) {
            if (t!=0) {
                auto [y, m, d, H, M, S]=spdFunc::decompEpoch(t);
                vDay.emplace_back(PicDay({y, m, d}));
            }
        }

        if (vDay.size()>0) {

            std::map<long, std::set<long> > mlslMonth;

            PicDay Day;
            unsigned int n=0;

            int vDaySize=vDay.size();
            for(int i=0; i<vDaySize-1; i++) {
                Day=vDay[i];
                n=0;
                while(Day.Y==vDay[i+n].Y &&
                      Day.M==vDay[i+n].M &&
                      Day.D==vDay[i+n].D   )  if (i+n<vDay.size()) n++;
                i+=n;
                vStat.emplace_back(PicStat({Day.Y, Day.M, Day.D, static_cast<long>(n)}));
            }

            std::set<long> sY, sM, sD;
            for(const auto &t: vStat) {
                sY.insert(t.Y);
                sM.insert(t.M);
                sD.insert(t.D);
            }

            ui->cbYear->clear();
            for(const auto &t: sY)
                ui->cbYear->addItem(QString::fromStdString(std::to_string(t)));

            if (this->lHistCurYear!=0)
                ui->cbYear->setCurrentText(QString::fromStdString(std::to_string(this->lHistCurYear)));

            for(const auto &Y: sY) {
                std::set<long> sM_tmp;
                for(unsigned int i=0; i<vStat.size(); i++) {
                    if (Y==vStat[i].Y)
                        sM_tmp.insert(vStat[i].M);
                }
                mlslMonth.insert({Y, sM_tmp});
            }

            long y=0;
            if (this->lHistCurYear!=0) y=this->lHistCurYear;
            else if (!ui->cbYear->currentText().isEmpty())
                y=std::stol(ui->cbYear->currentText().toStdString());

            this->setLogtext(tr("- Select year: ").toStdString()+std::to_string(y)+".\n");

            if (y!=0) {
                this->Chart->setTheme(QtCharts::QChart::ChartThemeBlueIcy);

                ui->cbYear->setHidden(false);
                this->chartView->setHidden(false);

                this->Chart->removeAllSeries();

                int MaxPic=0;

                for(auto &m: mlslMonth[y]) {
                    QBarSeries *series = new QBarSeries();
                    series->setName(QString::fromStdString("Month: "+std::to_string(m)));
                    for(int d=1;d<32;d++) {
                        QBarSet *set = new QBarSet(QString::fromStdString("Day: "+std::to_string(d)));

                        auto it=std::find_if(vStat.begin(), vStat.end(),
                                             [&](const auto &t) { return t.Y==y && t.M==m && t.D==d;});

                        if (it!=vStat.end()) {
                            int N=vStat[std::distance(vStat.begin(), it)].n;
                            if (N>MaxPic) MaxPic=N;
                            *set << N ;
                        }
                        else *set << 0;

                        set->setBorderColor(Qt::blue);
                        set->setBrush(QBrush(Qt::darkBlue));

                        series->append(set);
                    }
                    series->setUseOpenGL(true);
                    Chart->addSeries(series);
                }

                QStringList categories;
                for(auto &m: mlslMonth[y]) {
                    QString MonthStr;

                    switch (m) {
                        case 1: MonthStr=tr("Janvier"); break;
                        case 2: MonthStr=tr("February"); break;
                        case 3: MonthStr=tr("March"); break;
                        case 4: MonthStr=tr("April"); break;
                        case 5: MonthStr=tr("May"); break;
                        case 6: MonthStr=tr("June"); break;
                        case 7: MonthStr=tr("July"); break;
                        case 8: MonthStr=tr("August"); break;
                        case 9: MonthStr=tr("September"); break;
                        case 10: MonthStr=tr("October"); break;
                        case 11: MonthStr=tr("November"); break;
                        case 12: MonthStr=tr("December"); break;
                        default: break;
                    }
                    categories << MonthStr;
                }

                this->Chart->createDefaultAxes();

                QString HFontfamily(this->Chart->axes(Qt::Horizontal)[0]->titleFont().family());
                QString VFontfamily(this->Chart->axes(Qt::Vertical)[0]->titleFont().family());

                if (!this->Chart->axes(Qt::Horizontal).empty())
                    this->Chart->removeAxis(this->Chart->axes(Qt::Horizontal)[0]);

                this->axisX = new QBarCategoryAxis();
                this->axisX->append(categories);
                this->axisX->setLabelsFont(QFont(HFontfamily, 8));
                this->axisX->setLabelsColor(Qt::darkCyan);

                this->Chart->axes(Qt::Vertical)[0]->setTitleText(tr("pics/day"));
                this->Chart->axes(Qt::Vertical)[0]->setTitleFont(QFont(VFontfamily, 8));
                this->Chart->axes(Qt::Vertical)[0]->setLabelsFont(QFont(VFontfamily, 8));
                this->Chart->axes(Qt::Vertical)[0]->setLabelsColor(Qt::blue);
                this->Chart->axes(Qt::Vertical)[0]->setTitleBrush(QBrush(Qt::darkCyan));
                this->Chart->axes(Qt::Vertical)[0]->setRange(0, MaxPic);

                this->Chart->addAxis(this->axisX, Qt::AlignBottom);

                this->Chart->legend()->setVisible(false);
                this->Chart->setBackgroundBrush(QBrush(QColor(Qt::transparent)));
            }
        }
    }
    else this->setLogtext(tr("- Empty epoch vector.\n").toStdString());
}



void MainWIndow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange)
      ui->retranslateUi(this);
    else QWidget::changeEvent(event);
}

void MainWIndow::mousePressEvent(QMouseEvent *event) {
    if (!this->chartView->isHidden()) {
        if (this->curHistTheme==Theme::DARK) {
            this->setLogtext(tr("- Chose Qt theme for the histogram").toStdString()+".\n");
            this->curHistTheme=Theme::QT;
            this->Chart->setTheme(QtCharts::QChart::ChartThemeQt);
        }
        else if (this->curHistTheme==Theme::QT) {
            this->setLogtext(tr("- Chose BlueCerulean theme for the histogram").toStdString()+".\n");
            this->curHistTheme=Theme::BLUE;
            this->Chart->setTheme(QtCharts::QChart::ChartThemeBlueCerulean);
        }
        else if (this->curHistTheme==Theme::BLUE) {
            this->setLogtext(tr("- Chose Light theme for the histogram").toStdString()+".\n");
            this->curHistTheme=Theme::LIGHT;
            this->Chart->setTheme(QtCharts::QChart::ChartThemeLight);
        }
        else if (this->curHistTheme==Theme::LIGHT) {
            this->setLogtext(tr("- Chose Dark theme for the histogram").toStdString()+".\n");
            this->curHistTheme=Theme::DARK;
            this->Chart->setTheme(QtCharts::QChart::ChartThemeDark);
        }
        else {
            this->setLogtext(tr("- Chose Dark theme for the histogram").toStdString()+".\n");
            this->curHistTheme=Theme::DARK;
            this->Chart->setTheme(QtCharts::QChart::ChartThemeDark);
        }
        this->Chart->axes(Qt::Horizontal)[0]->setLabelsFont(QFont(this->Chart->axes(Qt::Horizontal)[0]->titleFont().family(), 8));
        this->Chart->axes(Qt::Vertical)[0]->setLabelsFont(QFont(this->Chart->axes(Qt::Vertical)[0]->labelsFont().family(), 8));
        this->Chart->axes(Qt::Vertical)[0]->setTitleFont(QFont(this->Chart->axes(Qt::Vertical)[0]->titleFont().family(), 8));
    }
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
            else {
                emit(sendEpoch(Epoch));
                emit(sendstdStr(QString::fromStdString("\t"+str.path().string()+"\t"+sTmp+"\n")));
            }
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
#ifdef IOSLEEP
            QThread::msleep(IOSLEEP);
#endif
        }
        emit(sendProgress(static_cast<float>((iCount++)*100/iFileNb)));
    }
    emit(sendProgress(100));
    emit(sendstdStr(tr("- Shifting completed.\n")));
    emit(finished());
}

void runShift::setDiff(long t) { this->Diff=t; }
void runShift::setDST(bool isDST) { this->bIsDST=isDST; }


