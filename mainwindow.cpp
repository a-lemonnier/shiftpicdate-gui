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
, isAutoScroll(true)
, iPicRot(-90)
, currentPic(0){
    ui->setupUi(this);
    
    this->setLogtext("\n");
    
    //ui->bTest->setHidden(true);
    
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
    
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.setViewMode(QFileDialog::List);
    
    
    // See on QT forum ***
    QListView *list = dialog.findChild<QListView*>("listView");
    if (list)  list->setSelectionMode(QAbstractItemView::MultiSelection);
    QTreeView *tree = dialog.findChild<QTreeView*>();
    if (tree)  tree->setSelectionMode(QAbstractItemView::MultiSelection);
    // ***
    
    QStringList fileNames={"./"};
    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    
    
    ui->lEPath->setText(fileNames[0]);
    ui->lEPath->setEnabled(false);
    
    this->setLogtext("- Path selected: ");
    for(const auto &str: fileNames)
        this->setLogtext(str.toUtf8().toStdString()+"\n");
    
    if (std::filesystem::is_directory(fileNames[0].toUtf8().toStdString())) {
        this->sFilename=fileNames[0].toUtf8().toStdString();
        
        // if one pic has been selected then display it
        auto fileNb=[](std::filesystem::path path)  {
            return std::distance(std::filesystem::recursive_directory_iterator{path},
                                 std::filesystem::recursive_directory_iterator{});
        };
        
        if (fileNb(this->sFilename)==1) {
            QString oneFilename=QString::fromStdString((*std::filesystem::recursive_directory_iterator(this->sFilename)).path().string());
            
            this->setLogtext("- Load image "+oneFilename.toStdString()+": ");
            
            ui->bReset->setEnabled(true);
            QImageReader* imgr=new QImageReader(oneFilename);
            imgr->read();
            
            this->setLogtext(imgr->errorString().toStdString()+"\n");
            
            QPixmap pmap(oneFilename);
            ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(),ui->picLabel->size().width(),Qt::KeepAspectRatio));
            
            delete imgr;
        }
        else {
            this->setEnabled(false);
            this->getfileList();
            this->setEnabled(true);
            
            this->startSlideshow();
        }
        
        ui->bRun->setEnabled(true);
        ui->bReset->setEnabled(true);
        ui->bQuit->setEnabled(true);
    }
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
    ui->bReset->setEnabled(true);
    
    qDebug() << " Test ---\n";



    qDebug() << "---\n";
}

void MainWIndow::on_bRot_clicked() {
    if (!this->timer_ss->isActive()) {
        this->iPicRot=90;
        QMatrix rm;
        rm.rotate(this->iPicRot);

        QPixmap cPmap=ui->picLabel->pixmap(Qt::ReturnByValue);
        cPmap = cPmap.transformed(rm);

        ui->picLabel->setPixmap(cPmap);
        this->iPicRot=0;
    }
    else
        this->iPicRot=90;
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
    ui->cBHidepic->setChecked(false);
    
    QString savedStrp="<html><head/><body><p><span style=\" font-weight:600; color:#aa0000;\">";
    QString savedStr=tr("No picture selected.");
    QString savedStrs="</span></p></body></html>";
    
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
    qDebug() << "Auto scroll: " << this->isAutoScroll << "\n";
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
    connect(timer, &QTimer::timeout, this, &MainWIndow::update_Log);
    
    // Update textBrowser at least one time
    connect(th, &QThread::finished, this,
            [=](){ this->timer->stop(); qDebug() << "timer stopped.\n";
                this->update_Log();  qDebug() << "update log.\n"; });
    
    connect(fL, &fileList::fLProgress, this,  &MainWIndow::update_progressBar_value);
    connect(fL, &fileList::sendstdStr, this,  &MainWIndow::update_Log_value);
    connect(fL, &fileList::finished, th, &QThread::quit);
    connect(th, &QThread::finished, th, &QThread::deleteLater);
    connect(th, &QThread::finished, fL, &fileList::deleteLater);
    connect(th, &QThread::started, fL, &fileList::getList);
    
    this->setLogtextTh("- List of files...\n");
    
    timer->start(1000);
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



void MainWIndow::on_cBHidepic_clicked(bool checked) {
    ui->picLabel->setHidden(checked);
}


void MainWIndow::startSlideshow() {
    
    if (std::filesystem::is_directory(this->sFilename)) {
        // List of pics
        this->vsList.clear();
        for(auto &sFile: fs::recursive_directory_iterator(this->sFilename))
            if (spdFunc::test_ext(sFile.path().filename().string()))
                vsList.push_back(sFile.path().string());
            
        std::sort(this->vsList.begin(), this->vsList.end());
        this->picNb=vsList.size();
        // --

        // Show the first img
        QPixmap pmap(QString::fromStdString(this->vsList[0]));
        ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(), ui->picLabel->size().width(),Qt::KeepAspectRatio));
        // --

        // Timer --
        timer_ss=new QTimer(this);
        connect(timer_ss, &QTimer::timeout, this, &MainWIndow::changePic);
        timer_ss->start(1000);
        // --
    }
}


void MainWIndow::changePic() {
    QMatrix rm;
    rm.rotate(this->iPicRot);
    QPixmap pmap(QString::fromStdString(this->vsList[this->currentPic++]));
    pmap = pmap.transformed(rm);
    ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(),ui->picLabel->size().width(),Qt::KeepAspectRatio));
    if (this->currentPic>this->picNb)
        this->currentPic=0;

    ui->picLabel->setToolTipDuration(1000);
    ui->picLabel->setToolTip(QString::fromStdString(this->vsList[this->currentPic++]));
    ui->statusbar->showMessage(QString::fromStdString(this->vsList[this->currentPic++]));

    this->iPicRot=0;
}

void MainWIndow::on_bNext_clicked() {
    if (this->timer_ss->isActive())
        timer_ss->stop();

    this->currentPic++;
    if (this->currentPic>this->picNb-1)
        this->currentPic=0;

    QPixmap pmap(QString::fromStdString(this->vsList[this->currentPic]));

    ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(),ui->picLabel->size().width(),Qt::KeepAspectRatio));

    ui->picLabel->setToolTipDuration(1000);
    ui->picLabel->setToolTip(QString::fromStdString(this->vsList[this->currentPic]));
    ui->statusbar->showMessage(QString::fromStdString(this->vsList[this->currentPic]));

}

void MainWIndow::on_bPrev_clicked() {
    if (this->timer_ss->isActive())
        timer_ss->stop();

    this->currentPic--;
    if (this->currentPic<0)
        this->currentPic=this->picNb-1;

    QPixmap pmap(QString::fromStdString(this->vsList[this->currentPic]));

    ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(),ui->picLabel->size().width(),Qt::KeepAspectRatio));


    ui->picLabel->setToolTipDuration(1000);
    ui->picLabel->setToolTip(QString::fromStdString(this->vsList[this->currentPic]));
    ui->statusbar->showMessage(QString::fromStdString(this->vsList[this->currentPic]));
}

void MainWIndow::on_bStop_clicked() {
    if (this->timer_ss->isActive())
        timer_ss->stop();
}
