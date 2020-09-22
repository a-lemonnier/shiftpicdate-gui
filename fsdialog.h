#ifndef FSDIALOG_H
#define FSDIALOG_H

#include <QDialog>
#include <QObject>
#include <QThread>
#include <QTranslator>
#include <QMessageBox>
#include <QMetaType>
#include <QScrollBar>
#include <QTimer>
#include <QImageReader>
#include <QGraphicsView>
#include <QPixmap>
#include <QMatrix>
#include <QListWidgetItem>

#include <string>
#include <vector>

#include "mainwindow.h"

namespace Ui {
class fsDialog;
}

class fsDialog : public QDialog {
    Q_OBJECT

public:
    explicit fsDialog(QWidget *parent = nullptr);
    ~fsDialog();

    void setFilelist(const std::vector<std::string> &vsList);
    std::vector<std::string> getFilelist();

signals:
    void sendvector(std::vector<std::string> &);
    void sendEpoch(std::vector<long> &);
    void load_img(int);

private slots:
    void on_bOK_clicked();


private:
    Ui::fsDialog *ui;

    std::vector<std::string> vsList;

};

#endif // FSDIALOG_H
