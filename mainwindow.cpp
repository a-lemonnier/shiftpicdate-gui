#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWIndow::MainWIndow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWIndow)
, SLog(tr("- Init.\n"))
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
, currentPic(0) {
    qDebug() << tr("- MainWIndow::MainWIndow(): init main windows.");

    ui->setupUi(this);
    
    ui->tBLog->setAcceptRichText(true);
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
    
    ui->lEPath->setText(".");
}

MainWIndow::~MainWIndow() {
    delete ui;
}


void MainWIndow::on_bBrowse_clicked() {
    qDebug() << tr("- MainWIndow::on_bBrowse_clicked().");

    qDebug() << tr("- MainWIndow::on_bBrowse_clicked(): config dialog.");
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.setViewMode(QFileDialog::List);
    
    qDebug() << tr("- MainWIndow::on_bBrowse_clicked(): set dialog tricks.");
    // See on QT forum ***
    QListView *list = dialog.findChild<QListView*>("listView");
    if (list)  list->setSelectionMode(QAbstractItemView::MultiSelection);
    QTreeView *tree = dialog.findChild<QTreeView*>();
    if (tree)  tree->setSelectionMode(QAbstractItemView::MultiSelection);
    // ***
    
    qDebug() << tr("- MainWIndow::on_bBrowse_clicked(): grab filenames.");
    QStringList fileNames={"./"};
    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    
    ui->lEPath->setText(fileNames[0]);
    ui->lEPath->setEnabled(false);
    
    this->setLogtext(tr("- Path selected: ").toStdString());
    for(const auto &str: fileNames)
        this->setLogtext(str.toUtf8().toStdString()+"\n");
    
    qDebug() << tr("- MainWIndow::on_bBrowse_clicked(): double check file type.");

    if (fs::is_directory(fileNames[0].toUtf8().toStdString())) {

        qDebug() << tr("- MainWIndow::on_bBrowse_clicked(): this is a directory.");
        this->sFilename=fileNames[0].toUtf8().toStdString()+"/";
        qDebug() << tr("- MainWIndow::on_bBrowse_clicked(): path: ") << QString::fromStdString(this->sFilename);

        // if one pic has been selected then display it
        auto fileNb=[](std::filesystem::path path) {
            long long n=-1;
            try { n=std::distance(std::filesystem::recursive_directory_iterator{path}, std::filesystem::recursive_directory_iterator{}); }
            catch (fs::filesystem_error &err) {qDebug() << tr("- MainWIndow::on_bBrowse_clicked::fileNb(lambda): error ") << QString::fromStdString(err.what());}
            return n;
        };

        long long llFilenb=fileNb(this->sFilename);

        if (llFilenb>-1) {
            qDebug() << tr("- MainWIndow::on_bBrowse_clicked(): ") << fileNb(this->sFilename) << tr(" files.");
            if (llFilenb==1 && spdFunc::test_ext((*fs::recursive_directory_iterator(this->sFilename)).path().string())) {
                qDebug() << tr("- MainWIndow::on_bBrowse_clicked(): show only one file having a good ext.");
                QString oneFilename=QString::fromStdString((*fs::recursive_directory_iterator(this->sFilename)).path().string());

                this->setLogtext(tr("- Load image ").toStdString()+oneFilename.toStdString()+": ");

                ui->bReset->setEnabled(true);
                QImageReader* imgr=new QImageReader(oneFilename);
                imgr->read();

                this->setLogtext(imgr->errorString().toStdString()+"\n");

                QPixmap pmap(oneFilename);
                ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(),ui->picLabel->size().width(),Qt::KeepAspectRatio));

                delete imgr;
            }
            else {
                qDebug() << tr("- MainWIndow::on_bBrowse_clicked(): start this->getfileList().");

                this->setEnabled(false);
                this->getfileList();
                this->setEnabled(true);

                qDebug() << tr("- MainWIndow::on_bBrowse_clicked(): start this->startSlideshow().");
                this->startSlideshow();
            }

            ui->bRun->setEnabled(true);
            ui->bReset->setEnabled(true);
            ui->bQuit->setEnabled(true);
        }
        else {
            qDebug() << tr("- \u25EC MainWIndow::on_bBrowse_clicked(): \u2620 Cannot process with this path: ") << QString::fromStdString(this->sFilename);
            this->setLogtext(tr("- \u25EC Cannot process with this path: ").toStdString()+this->sFilename+".\n");
        }
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
    qDebug() << tr("- MainWIndow::on_bDST_clicked().");
    if (!checked) {
        this->isDST=false;
        this->setLogtext(tr("- DST is disable.\n").toStdString());
    }
    else {
        this->isDST=true;
        this->setLogtext(tr("- DST is enable: +1h.\n").toStdString());
    }
}


void MainWIndow::on_bTest_clicked() {
    ui->bReset->setEnabled(true);
    
    qDebug() << tr("bTest clicked ---");

    qDebug() << QString::fromStdString(spdFunc::getExifDate("IMG_4580.JPG"));


    qDebug() << "---";
}

void MainWIndow::on_bRot_clicked() {
    qDebug() << tr("- MainWIndow::on_bRot_clicked().");
    if (!this->timer_ss->isActive()) {
        qDebug() << "- MainWIndow::on_bRot_clicked(): Stop timer.";

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
        this->setLogtext(tr("- Auto wrap off.\n").toStdString());
    }
    else {
        ui->tBLog->setLineWrapMode(QTextEdit::WidgetWidth);
        this->setLogtext(tr("- Auto wrap on.\n").toStdString());
    }
}

void MainWIndow::on_cBQuiet_clicked(bool checked) {
    qDebug() << tr("-  MainWIndow::on_cBQuiet_clicked().");
    this->isQuiet=checked;
    if (!checked)
        this->setLogtext(tr("- Toggle verbosity on.\n").toStdString());
}

void MainWIndow::on_cBScroll_clicked(bool checked) {
    qDebug() << tr("-  MainWIndow::on_cBScroll_clicked().");
    this->isAutoScroll=checked;
    if (!this->isQuiet) {
        if (checked) this->setLogtext(tr("- Toggle auto scroll on.\n").toStdString());
        else         this->setLogtext(tr("- Toggle auto scroll off.\n").toStdString());
    }
}

void MainWIndow::on_bReset_clicked() {
    qDebug() << tr("- MainWIndow::on_bReset_clicked().");

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
    this->setLogtext(tr("-> Shift by ").toStdString()+
    spdFunc::stoyear( this->computeDeltaT())
    +tr(" or ").toStdString()
    +std::to_string(this->computeDeltaT())
    +"s.\n");
}

void MainWIndow::on_sBYear_valueChanged(int val) {
    
    ui->bRun->setEnabled(true);
    
    this->DeltaY=val;
    this->computeDeltaT();
    
    this->setLogtext(tr("-> Shift by ").toStdString()+
    spdFunc::stoyear( this->computeDeltaT())
    +tr(" or ").toStdString()
    +std::to_string(this->computeDeltaT())
    +"s.\n");
}

void MainWIndow::on_sBSec_valueChanged(int val) {
    ui->bRun->setEnabled(true);
    
    this->DeltaS=val;
    this->computeDeltaT();
    
    this->setLogtext(tr("-> Shift by ").toStdString()+
    spdFunc::stoyear( this->computeDeltaT())
    +" or "
    +std::to_string(this->computeDeltaT())
    +"s.\n");
    
}


void MainWIndow::on_sBMin_valueChanged(int val) {
    ui->bRun->setEnabled(true);
    
    this->DeltaM=val;
    this->computeDeltaT();
    
    this->setLogtext(tr("-> Shift by ").toStdString()+
    spdFunc::stoyear( this->computeDeltaT())
    +" or "
    +std::to_string(this->computeDeltaT())
    +"s.\n");
}

void MainWIndow::on_sBHour_valueChanged(int val) {
    ui->bRun->setEnabled(true);
    
    this->DeltaH=val;
    this->computeDeltaT();
    
    this->setLogtext(tr("-> Shift by ").toStdString()+spdFunc::stoyear( this->computeDeltaT())
    +" or "       +std::to_string(this->computeDeltaT())
    +"s.\n");
}

void MainWIndow::on_bZoomIn_clicked() {
    qDebug() << "-  MainWIndow::on_bZoomIn_clicked().";
    ui->tBLog->zoomIn(1);
}

void MainWIndow::on_bZoomOut_clicked() {
    qDebug() << "-  MainWIndow::on_bZoomOut_clicked().";
    ui->tBLog->zoomOut(1);
}

void MainWIndow::on_rBInfo_clicked() { // About...
    qDebug() << "-  MainWIndow::on_rBInfo_clicked().";

    int currentProgress=ui->progressBar->value();
    ui->progressBar->setValue(100);
    
    QString infoMsg;
    infoMsg= "shiftpicdate-gui 0.1\n\n";
    infoMsg+=tr("Qt5 GUI for changing picture timestamps.\n\n");
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
    qDebug() << "- MainWIndow::update_Log().";
    if (!this->isQuiet) {
        ui->tBLog->setText(this->SLog);
        if (this->isAutoScroll) {
            qDebug() << tr("- MainWIndow::update_Log(): Scroll to the end.");
            ui->tBLog->verticalScrollBar()->setValue(ui->tBLog->verticalScrollBar()->maximum());
        }
    }
}



void MainWIndow::getfileList() {
    qDebug() << "- MainWIndow::getfileList().";

    fileList *fL=new fileList();
    QThread *th=new QThread();
    
    // Update textBrowser every n sec
    timer=new QTimer(nullptr);
    
    fL->moveToThread(th);
    fL->setfileName(this->sFilename);
    
    // Attach slop to timer
    connect(timer, &QTimer::timeout, this, &MainWIndow::update_Log);
    
    // Update textBrowser at least one time
    connect(th, &QThread::finished, this, [=](){ this->timer->stop(); this->update_Log(); });
    
    connect(fL, &fileList::fLProgress, this,  &MainWIndow::update_progressBar_value);
    connect(fL, &fileList::sendstdStr, this,  &MainWIndow::update_Log_value);
    connect(fL, &fileList::finished, th, &QThread::quit);
    connect(th, &QThread::finished, th, &QThread::deleteLater);
    connect(th, &QThread::finished, fL, &fileList::deleteLater);
    connect(th, &QThread::started, fL, &fileList::getList);
    
    this->setLogtextTh(tr("- List of files...\n").toStdString());
    
    timer->start(1000);
    th->start();
}



void fileList::getList() {
    qDebug() << "- fileList::getList().";

    auto fileNb=[](std::filesystem::path path)  {
        return std::distance(std::filesystem::recursive_directory_iterator{path},
                             std::filesystem::recursive_directory_iterator{});
    };
    
    size_t iFileNb=fileNb(this->fileName);
    qDebug() << "- fileList::getList(): " << iFileNb << tr(" files.");

    qDebug() << "- fileList::getList(): emit(sendstdStr()).";
    emit(sendstdStr(QString::fromStdString(tr("\t - Number of files: ").toStdString()+std::to_string(iFileNb)+" -\n")));
    
    int iCount=0;
    emit(fLProgress(0)); // Set progressBar to 0
    
    qDebug() << tr("- fileList::getList(): parse recursive_directory_iterator().");
    for(const auto &str: fs::recursive_directory_iterator(this->fileName)) {
        qDebug() << tr("- fileList::getList(): emit(sendstdStr()).");
        emit(sendstdStr(QString::fromStdString("\t"+str.path().string()+" - "+spdFunc::getExifDate(str.path().generic_string())+"\n")));
        emit(fLProgress(static_cast<float>((iCount++)*100/iFileNb))); // Set progressBar to n %
    }
    qDebug() << tr("- fileList::getList(): recursive_directory_iterator() loop complete.");
    
    emit(fLProgress(100)); // Set progressBar to 0
    emit(finished());
     qDebug() << "- fileList::getList(): emit(finished()).";
    
}

void fileList::setfileName(std::string& str) {
    qDebug() << "- fileList::setfileName().";
    this->fileName=str;
}



void MainWIndow::on_cBHidepic_clicked(bool checked) {
    qDebug() << "- MainWIndow::on_cBHidepic_clicked().";
    ui->picLabel->setHidden(checked);
}


void MainWIndow::startSlideshow() {
    qDebug() << "- MainWIndow::startSlideshow().";
    
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
    qDebug() << "- MainWIndow::changePic().";

    this->currentPic++;
    if (this->currentPic>this->picNb-1)
        this->currentPic=0;
    QMatrix rm;
    rm.rotate(this->iPicRot);
    QPixmap pmap(QString::fromStdString(this->vsList[this->currentPic]));
    pmap = pmap.transformed(rm);
    ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(),ui->picLabel->size().width(),Qt::KeepAspectRatio));

    ui->picLabel->setToolTipDuration(1000);
    ui->picLabel->setToolTip(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));
    ui->statusbar->showMessage(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));

    this->iPicRot=0;
}

void MainWIndow::on_bNext_clicked() {
    qDebug() << "- MainWIndow::on_bNext_clicked().";

    if (this->timer_ss->isActive())
        timer_ss->stop();

    this->currentPic++;
    if (this->currentPic>this->picNb-1)
        this->currentPic=0;

    QPixmap pmap(QString::fromStdString(this->vsList[this->currentPic]));

    ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(),ui->picLabel->size().width(),Qt::KeepAspectRatio));

    ui->picLabel->setToolTipDuration(1000);
    ui->picLabel->setToolTip(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));
    ui->statusbar->showMessage(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));

}

void MainWIndow::on_bPrev_clicked() {
    qDebug() << "- MainWIndow::on_bPrev_clicked().";

    if (this->timer_ss->isActive())
        timer_ss->stop();

    this->currentPic--;
    if (this->currentPic<0)
        this->currentPic=this->picNb-1;

    QPixmap pmap(QString::fromStdString(this->vsList[this->currentPic]));

    ui->picLabel->setPixmap(pmap.scaled(ui->picLabel->size().height(),ui->picLabel->size().width(),Qt::KeepAspectRatio));

    ui->picLabel->setToolTipDuration(1000);
    ui->picLabel->setToolTip(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));
    ui->statusbar->showMessage(QString::fromStdString(std::to_string(this->currentPic+1)+"/"+std::to_string(this->picNb)+"\t-\t"+this->vsList[this->currentPic]));
}

void MainWIndow::on_bStop_clicked() {
    qDebug() << "- MainWIndow::on_bStop_clicked()().";

    if (this->timer_ss->isActive())
        timer_ss->stop();
}
