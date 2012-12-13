/***************************************************************************
*   Copyright (C) 2006 by Kernel                                          *
*   kernelonline@bk.ru                                                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

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
};
#endif
