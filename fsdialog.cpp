#include "fsdialog.h"
#include "ui_fsdialog.h"

fsDialog::fsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fsDialog) {

    ui->setupUi(this);

    ui->bOK->setToolTip(tr("Confirm selection"));
    ui->bCancel->setToolTip(tr("Cancel selection"));
    ui->bSelectAll->setToolTip(tr("Select all pictures"));
    ui->bDeselectAll->setToolTip(tr("Deselect all pictures"));

    this->vsList.clear();
    for(const auto str: static_cast<MainWIndow*>(parent)->getvsList())
        this->vsList.emplace_back(str);

    for(const auto str: this->vsList)
        ui->listWidget->addItem(QString::fromStdString(str));

    connect(ui->listWidget, &QListWidget::itemClicked, this,
            [this](QListWidgetItem* lwItem) {
            QPixmap pmap(lwItem->text());
            ui->tlPic->setPixmap(pmap.scaled(ui->tlPic->size().height(), ui->tlPic->size().width(),Qt::KeepAspectRatio));
    } );
}

fsDialog::~fsDialog() { delete ui; }

void fsDialog::setFilelist(const std::vector<std::string> &vsList) {
    this->vsList=vsList;
}

std::vector<std::string> fsDialog::getFilelist() { return this->vsList; }

void fsDialog::on_bOK_clicked() {
    std::vector<std::string> vsTmp;

    for(int i=0;i<ui->listWidget->count();i++) {
        if (ui->listWidget->item(i)->isSelected())
            vsTmp.emplace_back(ui->listWidget->item(i)->text().toStdString());
    }

    if (!vsTmp.empty()) {
        this->vsList=vsTmp;
        emit(sendvector(this->vsList));
    }
    this->close();
}


