#ifndef MAINWINDOW_H
#define MAINWINDOW_H 1

#include <QtCore>
#include <QtGui>

#include "renderarea.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;

public:
    MainWindow(QWidget *parent = 0);
    
    QRenderArea *renderArea;
    QLabel* statusLabel;
    
    bool modified;
    
    int deletedTimer, repaintTimer;
    void generateConfigToFile(QTextStream & stream);
public slots:
    void deletedItem(QObject * obj);
    void timerEvent(QTimerEvent * event);
    void changingComponents(QCPBase * obj);
    
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileExit();
    void addComponent();
    void toolAllocate();
    void setZoom();
    
    void helpAbout();
protected:
    QString workFile;
    QString programTitle;
    QString loadingFile;

    void loadFile(QString & fname);
    bool saveFile(QString & fname);
    void closeEvent(QCloseEvent *event);
    void continueLoading();
    void updateStatus();
    void centerWindow();
};
#endif
