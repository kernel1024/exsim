#ifndef MAINWINDOW_H
#define MAINWINDOW_H 1

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include "renderarea.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
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

    void editCut();
    void editCopy();
    void editPaste();
    void editDelete();
    
    void helpAbout();
protected:
    QString workFile;
    QString programTitle;
    QString loadingFile;

    void loadFile(const QString &fname);
    bool saveFile(const QString &fname);
    void closeEvent(QCloseEvent *event);
    void continueLoading();
    void updateStatus();
    void centerWindow();
    void cleanupBlock(QDomElement & block);
};
#endif