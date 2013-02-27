#ifndef LOADINGLOGDIALOG_H
#define LOADINGLOGDIALOG_H

#include <QDialog>

namespace Ui {
class QLoadingLogDialog;
}

class QLoadingLogDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QLoadingLogDialog(QWidget *parent = 0);
    ~QLoadingLogDialog();
    void setLogText(QString log);
    
private:
    Ui::QLoadingLogDialog *ui;
};

#endif // LOADINGLOGDIALOG_H
