#include "loadinglogdialog.h"
#include "ui_loadinglogdialog.h"

QLoadingLogDialog::QLoadingLogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QLoadingLogDialog)
{
    ui->setupUi(this);
}

QLoadingLogDialog::~QLoadingLogDialog()
{
    delete ui;
}

void QLoadingLogDialog::setLogText(const QString &log)
{
    ui->logViewer->clear();
    ui->logViewer->append(log);
}
