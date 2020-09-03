#include "fsdialog.h"
#include "ui_fsdialog.h"

fsDialog::fsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fsDialog) {

    ui->setupUi(this);
    qDebug().noquote() << tr("- fsDialog::fsDialog(): window initialized.");

    qDebug().noquote() << "- fsDialog::fsDialog(): Cast: " << static_cast<MainWIndow*>(parent)->getvsList().size();// << " Copy: " << this->vsList.size();

    this->vsList.clear();
    for(const auto str: static_cast<MainWIndow*>(parent)->getvsList())
        this->vsList.emplace_back(str);

    qDebug().noquote() << tr("- fsDialog::fsDialog(): fill list with ") << this->vsList.size() << tr("elements.");
    for(const auto str: this->vsList)
        ui->listWidget->addItem(QString::fromStdString(str));

    // show clicked img on the right panel
    connect(ui->listWidget, &QListWidget::itemClicked, this,
            [=](QListWidgetItem* lwItem) {
            qDebug().noquote() << tr("- fsDialog::fsDialog(): item ") << lwItem->text() << tr(" selected.");
            QPixmap pmap(lwItem->text());
            ui->tlPic->setPixmap(pmap.scaled(ui->tlPic->size().height(), ui->tlPic->size().width(),Qt::KeepAspectRatio));
    } );

}

fsDialog::~fsDialog() {
    qDebug().noquote() << tr("- fsDialog::~fsDialog(): window closed.");
    delete ui;
}

void fsDialog::on_buttonBox_accepted() {
    qDebug().noquote() << tr("- fsDialog::on_buttonBox_accepted().");

    this->close();
}


void fsDialog::setFilelist(const std::vector<std::string> &vsList) {
    qDebug().noquote() << tr("- fsDialog::setFilelist(&&): copy: ") << vsList.size() << tr(" elements.");
    this->vsList=vsList;
}

std::vector<std::string> fsDialog::getFilelist() {
    return this->vsList;
}

void fsDialog::on_bOK_clicked() {

    qDebug().noquote() << tr("- fsDialog::on_bOK_clicked(): current size ") <<  (int)ui->listWidget->count() << ".";

    std::vector<std::string> vsTmp;

    for(int i=0;i<ui->listWidget->count();i++) {
        if (ui->listWidget->item(i)->isSelected()) {
            qDebug().noquote() << tr("- fsDialog::on_bOK_clicked(): keep ") << ui->listWidget->item(i)->text() << ".";
            vsTmp.emplace_back(ui->listWidget->item(i)->text().toStdString());
        }
    }

    if (!vsTmp.empty()) {
        this->vsList=vsTmp;
        qDebug().noquote() << tr("- fsDialog::on_bOK_clicked(): current size ") <<  this->vsList.size() << ".";
        emit(sendvector(this->vsList));
    }

    this->close();
}



