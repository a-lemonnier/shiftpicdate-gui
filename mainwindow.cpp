#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "shiftpicdate.h"

MainWIndow::MainWIndow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWIndow)
, SLog("- Init.\n")
, sFilename("./")
, DeltaY(0)
, DeltaD(0)
, DeltaH(0)
, DeltaM(0)
, DeltaS(0)
, DeltaT(0)
, isDST(false)
, isQuiet(false)
, isAutoScroll(true) {
    ui->setupUi(this);
    
    this->setLogtext("\n");
    
    ui->bTest->setHidden(true);

    ui->bRun->setEnabled(false);
    ui->bReset->setEnabled(false);

    ui->sBYear->setEnabled(true);
    ui->sBDay->setEnabled(true);
    ui->bDST->setEnabled(true);
    ui->bDST->setChecked(this->isDST);

    ui->cBScroll->setChecked(true);
    ui->cBWrap->setChecked(true);
    
    ui->sBYear->setValue(this->DeltaY);
    ui->sBDay->setValue(this->DeltaD);
    ui->sBHour->setValue(this->DeltaH);
    ui->sBMin->setValue(this->DeltaM);
    ui->sBSec->setValue(this->DeltaS);
    
    ui->lEPath->setText("./");
}

MainWIndow::~MainWIndow() {
    delete ui;
}



void MainWIndow::on_bBrowse_clicked() {
    
    QFileDialog dialog(this);

    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    dialog.setViewMode(QFileDialog::List);

    QStringList fileNames={"./"};
    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    
    ui->lEPath->setText(fileNames[0]);
    ui->lEPath->setEnabled(false);
    
    this->setLogtext("- Path selected: "+fileNames[0].toUtf8().toStdString()+"\n");
    this->sFilename=fileNames[0].toUtf8().toStdString();

    this->setEnabled(false);

    this->getfileList();

    this->setEnabled(true);
    ui->bRun->setEnabled(true);
    ui->bReset->setEnabled(true);
    ui->bQuit->setEnabled(true);

}


void MainWIndow::on_bRun_clicked() {
    ui->bQuit->setEnabled(false);
    ui->bBrowse->setEnabled(false);
    ui->bReset->setEnabled(false);
    ui->sBYear->setEnabled(false);
    ui->sBDay->setEnabled(false);
    ui->sBHour->setEnabled(false);
    ui->sBMin->setEnabled(false);
    ui->sBSec->setEnabled(false);

    this->setLogtext("- Run...");

}


void MainWIndow::on_bDST_clicked(bool checked) {
    if (!checked) {
        this->isDST=false;
        this->setLogtext("- DST is disable.\n");
    }
    else {
        this->isDST=true;
        this->setLogtext("- DST is enable: +1h.\n");
    }
}


void MainWIndow::on_bTest_clicked() {
    qDebug() << "bTest() clicked.\n";

}

void MainWIndow::on_cBWrap_clicked(bool checked) {
    if (!checked) {
        ui->tBLog->setLineWrapMode(QTextEdit::NoWrap);
        this->setLogtext("- Auto wrap off.\n");
    }
    else {
        ui->tBLog->setLineWrapMode(QTextEdit::WidgetWidth);
        this->setLogtext("- Auto wrap on.\n");
    }
}

void MainWIndow::on_cBQuiet_clicked(bool checked) {
    this->isQuiet=checked;
    if (!checked)
        this->setLogtext("- Toggle verbosity on.\n");
}

void MainWIndow::on_cBScroll_clicked(bool checked) {
    this->isAutoScroll=checked;
    if (!this->isQuiet) {
        if (checked) this->setLogtext("- Toggle auto scroll on.\n");
        else         this->setLogtext("- Toggle auto scroll off.\n");
    }
}

void MainWIndow::on_bReset_clicked() {
    ui->bRun->setEnabled(true);
    ui->sBYear->setEnabled(true);
    ui->sBDay->setEnabled(true);
    ui->bDST->setEnabled(true);
    ui->lEPath->setEnabled(true);
    
    ui->bDST->setChecked(this->isDST);
    
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
    
}

void MainWIndow::on_sBDay_valueChanged(int val) {
    
    ui->bRun->setEnabled(true);
    
    this->DeltaD=val;
    this->computeDeltaT();
    this->setLogtext("-> Shift by "+
    spdFunc::stoyear( this->computeDeltaT())
    +" or "
    +std::to_string(this->computeDeltaT())
    +"s.\n");
}

void MainWIndow::on_sBYear_valueChanged(int val) {
    
    ui->bRun->setEnabled(true);
    
    this->DeltaY=val;
    this->computeDeltaT();
    
    this->setLogtext("-> Shift by "+
    spdFunc::stoyear( this->computeDeltaT())
    +" or "
    +std::to_string(this->computeDeltaT())
    +"s.\n");
}

void MainWIndow::on_sBSec_valueChanged(int val) {
    ui->bRun->setEnabled(true);
    
    this->DeltaS=val;
    this->computeDeltaT();
    
    this->setLogtext("-> Shift by "+
    spdFunc::stoyear( this->computeDeltaT())
    +" or "
    +std::to_string(this->computeDeltaT())
    +"s.\n");
    
}


void MainWIndow::on_sBMin_valueChanged(int val) {
    ui->bRun->setEnabled(true);
    
    this->DeltaM=val;
    this->computeDeltaT();
    
    this->setLogtext("-> Shift by "+
    spdFunc::stoyear( this->computeDeltaT())
    +" or "
    +std::to_string(this->computeDeltaT())
    +"s.\n");
}

void MainWIndow::on_sBHour_valueChanged(int val) {
    ui->bRun->setEnabled(true);
    
    this->DeltaH=val;
    this->computeDeltaT();
    
    this->setLogtext("-> Shift by "+spdFunc::stoyear( this->computeDeltaT())
                     +" or "       +std::to_string(this->computeDeltaT())
                     +"s.\n");
}

void MainWIndow::on_bZoomIn_clicked() {
    ui->tBLog->zoomIn(1);
}

void MainWIndow::on_bZoomOut_clicked() {
    ui->tBLog->zoomOut(1);
}

void MainWIndow::on_rBInfo_clicked() { // About...
    int currentProgress=ui->progressBar->value();
    ui->progressBar->setValue(100);

    QString infoMsg;
    infoMsg= "shiftpicdate-gui 0.1\n\n";
    infoMsg+="Qt5 GUI for changing picture timestamps.\n\n";
    infoMsg+="MIT - A. Lemonnier - 2020\n";

    QMessageBox msgBox;
    msgBox.setSizePolicy(QSizePolicy::Policy::Expanding,
                         QSizePolicy::Policy::Expanding);
    msgBox.setText(infoMsg);

    msgBox.exec();

    ui->progressBar->setValue(currentProgress);
    ui->rBInfo->setChecked(false);
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
    ui->progressBar->setValue(v);
}

void MainWIndow::update_Log_value(QString str) {
    this->setLogtextTh(str.toUtf8().constData());
}

void MainWIndow::update_Log() {
    qDebug() << "timer tick.\n";
    //qDebug() << "Auto scroll: " << this->isAutoScroll << "\n";
    if (!this->isQuiet) {
        ui->tBLog->setText(this->SLog);
        if (this->isAutoScroll)
         ui->tBLog->verticalScrollBar()->setValue(ui->tBLog->verticalScrollBar()->maximum());
    }
}



void MainWIndow::getfileList() {
    fileList *fL=new fileList();
    QThread *th=new QThread();

    // Update textBrowser every n sec
    timer=new QTimer(nullptr);

    fL->moveToThread(th);
    fL->setfileName(this->sFilename);

    // Attach slop to timer
    connect(timer, &QTimer::timeout, this, &MainWIndow::update_Log); //,Qt::QueuedConnection);

    // Update textBrowser at least one time
    connect(th, &QThread::finished, this,
            [=](){ this->timer->stop(); qDebug() << "timer stopped.\n";
                   this->update_Log();  qDebug() << "update log.\n"; }); //, Qt::QueuedConnection);

    connect(fL, &fileList::fLProgress, this,  &MainWIndow::update_progressBar_value);
    connect(fL, &fileList::sendstdStr, this,  &MainWIndow::update_Log_value);

    connect(fL, &fileList::finished, th, &QThread::quit);

    connect(th, &QThread::finished, th, &QThread::deleteLater);
    connect(th, &QThread::finished, fL, &fileList::deleteLater);

    connect(th, &QThread::started, fL, &fileList::getList);

//    connect(th, &QThread::started, this, [=]() {qDebug() << "start thread.\n";});
//    connect(fL, &fileList::finished, this, [=]() {qDebug() << "end thread.\n";});

    this->setLogtextTh("- List of files...\n");

    timer->start(1500);
    th->start();

}



void fileList::getList() {

    auto fileNb=[](std::filesystem::path path)  {
        return std::distance(std::filesystem::recursive_directory_iterator{path},
                             std::filesystem::recursive_directory_iterator{});
    };

    size_t iFileNb=fileNb(this->fileName);

    emit(sendstdStr(QString::fromStdString("\t - Number of files: "+std::to_string(iFileNb)+" -\n")));

    int iCount=0;
    emit(fLProgress(0)); // Set progressBar to 0

    for(const auto &str: std::filesystem::recursive_directory_iterator(this->fileName)) {

        emit(sendstdStr(QString::fromStdString("\t"+str.path().string()+"\n")));
        emit(fLProgress(static_cast<float>((iCount++)*100/iFileNb))); // Set progressBar to n %
    }

    emit(fLProgress(100)); // Set progressBar to 0
    emit(finished());

}

void fileList::setfileName(std::string& str) {
    this->fileName=str;
}


