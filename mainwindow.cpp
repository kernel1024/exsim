#include <stdlib.h>
#include <time.h>
#include "mainwindow.h"
#include "components/cpbase.h"
#include <AL/al.h>
#include <AL/alc.h>

void init_al() {
    ALCdevice *dev = NULL;
    ALCcontext *ctx = NULL;

    const char *defname = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

    dev = alcOpenDevice(defname);
    ctx = alcCreateContext(dev, NULL);
    alcMakeContextCurrent(ctx);
}

void exit_al() {
    ALCdevice *dev = NULL;
    ALCcontext *ctx = NULL;
    ctx = alcGetCurrentContext();
    dev = alcGetContextsDevice(ctx);

    alcMakeContextCurrent(NULL);
    alcDestroyContext(ctx);
    alcCloseDevice(dev);
}

MainWindow::MainWindow(QWidget * parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init_al();

    QIcon appIcon;
    // Icon author: Jack Cai
    appIcon.addFile(":/images/led16.png");
    appIcon.addFile(":/images/led32.png");
    appIcon.addFile(":/images/led48.png");
    appIcon.addFile(":/images/led64.png");
    appIcon.addFile(":/images/led128.png");
    setWindowIcon(appIcon);

    resize(QSize(800, 500).expandedTo(minimumSizeHint()));

    statusLabel=new QLabel();
    this->statusBar()->addPermanentWidget(statusLabel);

    renderArea = new QRenderArea(ui->scrollArea,ui->scrollArea);
    ui->scrollArea->setWidget(renderArea);

    programTitle=tr("exSim electronic simulator");
    workFile="";

    repaintTimer=0;
    deletedTimer=0;

    ui->toolBarFile->addAction(ui->actionNew);
    ui->toolBarFile->addAction(ui->actionOpen);
    ui->toolBarFile->addAction(ui->actionSave);
    ui->toolBarComponents->addAction(ui->actionButton);
    ui->toolBarComponents->addAction(ui->actionLED);
    ui->toolBarComponents->addAction(ui->actionBeeper);
    ui->toolBarComponents->addAction(ui->actionOutputs_extender);
    ui->toolBarComponents->addAction(ui->actionOscillator);
    ui->toolBarComponents->addSeparator();
    ui->toolBarComponents->addAction(ui->actionLogic);
    ui->toolBarComponents->addAction(ui->actionTrigger);

    connect(ui->actionNew,SIGNAL(triggered()),this,SLOT(fileNew()));
    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(fileOpen()));
    connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(fileSave()));
    connect(ui->actionSave_as,SIGNAL(triggered()),this,SLOT(fileSaveAs()));
    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(fileExit()));
    connect(ui->actionZoom,SIGNAL(triggered()),this,SLOT(setZoom()));
    connect(ui->actionCompact_layout,SIGNAL(triggered()),this,SLOT(toolAllocate()));
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(helpAbout()));

    connect(ui->actionButton,SIGNAL(triggered()),this,SLOT(addComponent()));
    connect(ui->actionLED,SIGNAL(triggered()),this,SLOT(addComponent()));
    connect(ui->actionOutputs_extender,SIGNAL(triggered()),this,SLOT(addComponent()));
    connect(ui->actionLogic,SIGNAL(triggered()),this,SLOT(addComponent()));
    connect(ui->actionTrigger,SIGNAL(triggered()),this,SLOT(addComponent()));
    connect(ui->actionOscillator,SIGNAL(triggered()),this,SLOT(addComponent()));
    connect(ui->actionBeeper,SIGNAL(triggered()),this,SLOT(addComponent()));


    modified=false;
    updateStatus();

    srand(clock());

    centerWindow();
}

MainWindow::~MainWindow()
{
    exit_al();
}

void MainWindow::updateStatus()
{
    if (modified)
        statusLabel->setText(tr("Modified"));
    else
        statusLabel->setText("");
    
    QFileInfo fi(workFile);
    QString s=fi.fileName();
    if (workFile=="") s=tr("[untitled]");
    if (modified) s+=" *";
    s=s+" - "+programTitle;

    setWindowTitle(s);
}

void MainWindow::centerWindow()
{
    int screen = 0;
    QWidget *w = window();
    QDesktopWidget *desktop = QApplication::desktop();
    if (w) {
        screen = desktop->screenNumber(w);
    } else if (desktop->isVirtualDesktop()) {
        screen = desktop->screenNumber(QCursor::pos());
    } else {
        screen = desktop->screenNumber(this);
    }
    QRect rect(desktop->availableGeometry(screen));
    int h = 60*rect.height()/100;
    QSize nw(135*h/100,h);
    resize(nw);
    move(rect.width()/2 - frameGeometry().width()/2,
         rect.height()/2 - frameGeometry().height()/2);
}

void MainWindow::deletedItem(QObject *)
{
    if (renderArea->cpComponentCount()==0) deletedTimer=startTimer(250);
}

void MainWindow::timerEvent(QTimerEvent * event)
{
    if (event->timerId()==repaintTimer)
    {
        killTimer(repaintTimer);
        repaintTimer=0;
        renderArea->repaintConn();
        update();
    }
    else if (event->timerId()==deletedTimer)
    {
        killTimer(deletedTimer);
        continueLoading();
    }
}

void MainWindow::loadFile(QString & fname)
{
    loadingFile=fname;
    if (renderArea->cpComponentCount()>0)
    {
        for (int i=0;i<renderArea->children().count();i++)
        {
            QCPBase* base;
            if (!(base=qobject_cast<QCPBase*>(renderArea->children().at(i)))) continue;
            connect(base,SIGNAL(destroyed(QObject*)),this,SLOT(deletedItem(QObject*)));
        }
        renderArea->deleteComponents();
    } else
        continueLoading();
}

bool MainWindow::saveFile(QString & fname)
{
    bool allOk=true;
    QFile file(fname);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this,"Error","Cannot create file");
        return false;
    }
    QDataStream out(&file);
    try
    {
        renderArea->storeSchematic(out);
    }
    catch (const char* p)
    {
        QMessageBox::critical(this,"Error",QString::fromAscii(p));
        allOk=false;
    }
    file.close();
    return allOk;
}

void MainWindow::continueLoading()
{
    QFile file(loadingFile);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this,"Error","Cann't read file");
        return;
    }
    QDataStream ins(&file);
    try
    {
        renderArea->readSchematic(ins);
        for (int i=0;i<renderArea->children().count();i++)
        {
            QCPBase* base;
            if (!(base=qobject_cast<QCPBase*>(renderArea->children().at(i)))) continue;
            connect(base,SIGNAL(componentChanged(QCPBase*)),this,SLOT(changingComponents(QCPBase*)));
        }
    }
    catch (const char* p)
    {
        renderArea->deleteComponents();
        QMessageBox::critical(this,"Error",QString::fromAscii(p));
    }
    file.close();
    repaintTimer=startTimer(500);
}

void MainWindow::fileNew()
{
    if (modified)
    {
        switch (QMessageBox::question(this,tr("New file"),
                                      tr("Current file has been modified and not saved. Save?"),
                                      QMessageBox::Yes,QMessageBox::No,QMessageBox::Cancel))
        {
            case QMessageBox::Cancel:
                return;
            case QMessageBox::Yes:
                fileSave();
                if (modified) return;
                break;
        }
    }

    ui->scrollArea->ensureVisible(0,0);
    renderArea->deleteComponents();
    modified=false;
    workFile="";
    updateStatus();
    repaintTimer=startTimer(1000);
}

void MainWindow::fileOpen()
{
    if (modified)
    {
        switch (QMessageBox::question(this,tr("Open file"),
                                      tr("Current file has been modified and not saved. Save?"),
                                      QMessageBox::Yes,QMessageBox::No,QMessageBox::Cancel))
        {
            case QMessageBox::Cancel:
                return;
            case QMessageBox::Yes:
                fileSave();
                if (modified) return;
                break;
        }
    }
    QString s = QFileDialog::getOpenFileName(
                    this,
                    tr("Choose a file"),
                    "",
                    "exSim files (*.exs)");
    if (s.isEmpty()) return;
    workFile=s;
    modified=false;
    loadFile(s);
    updateStatus();
}

void MainWindow::fileSave()
{
    if (!modified) return;
    if (workFile=="")
    {
        QString fname = QFileDialog::getSaveFileName(this,tr("Choose a filename to save under"),QString(),
                                                     tr("exSim files (*.exs)"));
        if (fname.isEmpty()) return;
        QFileInfo fi(fname);
        if (fi.suffix().isEmpty())
            fname+=".exs";
        workFile=fname;
    }
    if (!saveFile(workFile))
        QMessageBox::critical(this,"Error","File can't be saved");
    else
        modified=false;
    updateStatus();
}

void MainWindow::fileSaveAs()
{
    QString fname = QFileDialog::getSaveFileName(this,tr("Choose a filename to save under"),QString(),
                                                 tr("exSim files (*.exs)"));
    if (fname.isEmpty()) return;
    QFileInfo fi(fname);
    if (fi.suffix().isEmpty())
        fname+=".exs";
    workFile=fname;
    modified=true;
    fileSave();
}

void MainWindow::fileExit()
{
    close();
}

void MainWindow::helpAbout()
{
    QMessageBox::about(this,"exSim electronic simulator","exSim electronic simulator\n\n"
                       "(c) 2012 by Kernel\n\n"
                       "This program is provided AS IS with NO WARRANTY OF ANY KIND,\n"
                       "INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS\n"
                       "FOR A PARTICULAR PURPOSE.\n\n"
                       "This program distributed under GPL license.\n\n"
                       "This program is free software; you can redistribute it and/or\n"
                       "modify it under the terms of the GNU General Public License as\n"
                       "published by the Free Software Foundation; either version 3 of\n"
                       "the License, or (at your option) any later version. ");
}

void MainWindow::changingComponents(QCPBase *)
{
    modified=true;
    updateStatus();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!modified)
    {
        event->accept();
        return;
    }
    switch (QMessageBox::question(this,tr("Exit exSim"),
                                  tr("Current file has been modified and not saved. Save?"),
                                  QMessageBox::Yes,QMessageBox::No,QMessageBox::Cancel))
    {
        case QMessageBox::Cancel:
            event->ignore();
            return;
        case QMessageBox::Yes:
            fileSave();
            if (modified)
            {
                event->ignore();
                return;
            }
            break;
    }
    event->accept();
}

// Components tools

void MainWindow::addComponent()
{
    QAction* ac = qobject_cast<QAction*>(sender());
    if (ac==NULL) return;
    if (ac->whatsThis().isEmpty()) return;
    QCPBase* a= renderArea->createCpInstance(ac->whatsThis());
    if (a==NULL) return;
    a->move(100,100);
    a->setObjectName(ac->whatsThis()+QString::number(renderArea->cpComponentCount(),16));
    a->show();
    connect(a,SIGNAL(componentChanged(QCPBase *)),this,SLOT(changingComponents(QCPBase *)));
    changingComponents(a);
    update();
}

void MainWindow::toolAllocate()
{
    for (int i=0;i<renderArea->children().count();i++)
        if (QWidget* w=qobject_cast<QWidget*>(renderArea->children().at(i)))
        {
            if ((w->x()<0) || (w->geometry().right()>renderArea->width()))
                w->move(100,w->y());
            if ((w->y()<0) || (w->geometry().bottom()>renderArea->height()))
                w->move(w->x(),100);
        }
    renderArea->update();
    renderArea->repaintConn();
}

void MainWindow::setZoom()
{
    bool ok;
    int z = QInputDialog::getInt(this,tr("Adjust current zoom"),tr("Zoom (%)"),
                                 renderArea->zoom,10,500,10,&ok);
    if (ok)
        renderArea->setZoom(z);

}