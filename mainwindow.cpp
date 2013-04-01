#include <stdlib.h>
#include <time.h>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QScrollBar>
#include <QInputDialog>
#include <QFontDatabase>
#include <QClipboard>
#include "openal.h"
#include "mainwindow.h"
#include "components/cpbase.h"
#include "loadinglogdialog.h"

bool MainWindow::initOpenAL() {
    const char *defname = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

    alDev = alcOpenDevice(defname);
    if (alDev == NULL)
        return false;

    alCtx = alcCreateContext(alDev, NULL);
    if (alCtx == NULL) {
        alcCloseDevice(alDev);
        alDev = NULL;
        return false;
    }
    if (!alcMakeContextCurrent(alCtx)) {
        alcDestroyContext(alCtx);
        alcCloseDevice(alDev);
        alCtx = NULL;
        alDev = NULL;
        return false;
    }
    return true;
}

void MainWindow::doneOpenAL() {
    if (alCtx != NULL) {
        alcMakeContextCurrent(NULL);
        alcDestroyContext(alCtx);
    }
    if (alDev !=NULL)
        alcCloseDevice(alDev);
    alCtx = NULL;
    alDev = NULL;
}

MainWindow::MainWindow(QWidget * parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    alDev = NULL;
    alCtx = NULL;

    initOpenAL();

    QIcon appIcon;
    // Icon author: Jack Cai
    appIcon.addFile(":/images/led16.png");
    appIcon.addFile(":/images/led32.png");
    appIcon.addFile(":/images/led48.png");
    appIcon.addFile(":/images/led64.png");
    appIcon.addFile(":/images/led128.png");
    setWindowIcon(appIcon);

    int fontIdx = -1;
    QFile res(":/images/digital.ttf");
    if (res.open(QIODevice::ReadOnly)) {
        fontIdx = QFontDatabase::addApplicationFontFromData(res.readAll());
        res.close();
    }

    resize(QSize(800, 500).expandedTo(minimumSizeHint()));

    statusLabel=new QLabel();
    this->statusBar()->addPermanentWidget(statusLabel);

    renderArea = new QRenderArea(ui->scrollArea,ui->scrollArea,fontIdx,this);
    ui->scrollArea->setWidget(renderArea);

    programTitle=tr("exSim electronic simulator");
    workFile="";

    repaintTimer=0;
    deletedTimer=0;

#ifdef WIN32
    ui->actionNew->setIcon(QIcon(":/images/theme-icons/document-new.png"));
    ui->actionOpen->setIcon(QIcon(":/images/theme-icons/document-open.png"));
    ui->actionSave->setIcon(QIcon(":/images/theme-icons/document-save.png"));
    ui->actionSave_as->setIcon(QIcon(":/images/theme-icons/document-save-as.png"));
    ui->actionExit->setIcon(QIcon(":/images/theme-icons/application-exit.png"));
    ui->actionCut->setIcon(QIcon(":/images/theme-icons/edit-cut.png"));
    ui->actionCopy->setIcon(QIcon(":/images/theme-icons/edit-copy.png"));
    ui->actionPaste->setIcon(QIcon(":/images/theme-icons/edit-paste.png"));
    ui->actionDelete->setIcon(QIcon(":/images/theme-icons/edit-delete.png"));
    ui->actionZoom->setIcon(QIcon(":/images/theme-icons/zoom-draw.png"));
#endif

    ui->toolBarFile->addAction(ui->actionNew);
    ui->toolBarFile->addAction(ui->actionOpen);
    ui->toolBarFile->addAction(ui->actionSave);
    ui->toolBarFile->addSeparator();
    ui->toolBarFile->addAction(ui->actionCut);
    ui->toolBarFile->addAction(ui->actionCopy);
    ui->toolBarFile->addAction(ui->actionPaste);

    connect(ui->actionNew,SIGNAL(triggered()),this,SLOT(fileNew()));
    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(fileOpen()));
    connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(fileSave()));
    connect(ui->actionSave_as,SIGNAL(triggered()),this,SLOT(fileSaveAs()));
    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(fileExit()));
    connect(ui->actionZoom,SIGNAL(triggered()),this,SLOT(setZoom()));
    connect(ui->actionCompact_layout,SIGNAL(triggered()),this,SLOT(toolAllocate()));
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(helpAbout()));
    connect(ui->actionCut,SIGNAL(triggered()),this,SLOT(editCut()));
    connect(ui->actionCopy,SIGNAL(triggered()),this,SLOT(editCopy()));
    connect(ui->actionPaste,SIGNAL(triggered()),this,SLOT(editPaste()));
    connect(ui->actionDelete,SIGNAL(triggered()),this,SLOT(editDelete()));

    for(int i=0;i<ui->menuComponents->actions().count();i++) {
        QAction* acm = ui->menuComponents->actions().at(i);
        if (!acm->isSeparator()) {
            ui->toolBarComponents->addAction(acm);
            connect(acm,SIGNAL(triggered()),this,SLOT(addComponent()));
        } else
            ui->toolBarComponents->addSeparator();
    }

    modified=false;
    updateStatus();

    qsrand(clock());

    centerWindow();
}

MainWindow::~MainWindow()
{
    doneOpenAL();
}

bool MainWindow::isSoundOK()
{
    return ((alDev != NULL) && (alCtx != NULL));
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

void MainWindow::loadFile(const QString & fname)
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

bool MainWindow::saveFile(const QString &fname)
{
    QFile file(fname);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this,"Error","Cannot create file");
        return false;
    }
    QTextStream fout(&file);

    QDomDocument xdoc;
    QDomElement clist = xdoc.createElement("components");
    renderArea->storeSchematic(clist);
    xdoc.appendChild(clist);

    xdoc.save(fout,4);
    file.close();
    return true;
}

void MainWindow::continueLoading()
{
    QFile file(loadingFile);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this,"exSim error","Can't read file");
        return;
    }
    QDomDocument xdoc;
    int eline, ecol;
    QString emsg;
    if (!xdoc.setContent(&file,&emsg,&eline,&ecol)) {
        QMessageBox::critical(this,"exSim XML error",tr("XML parsing error:\n %1\n at line %2, column %3.").
                              arg(emsg).arg(eline).arg(ecol));
        file.close();
        return;
    }
    file.close();
    QByteArray errbuf;
    errbuf.clear();
    QTextStream errlog(&errbuf);

    renderArea->readSchematic(errlog,xdoc.firstChildElement("components"));
    for (int i=0;i<renderArea->children().count();i++) {
        QCPBase* base;
        if (!(base=qobject_cast<QCPBase*>(renderArea->children().at(i)))) continue;
        connect(base,SIGNAL(componentChanged(QCPBase*)),this,SLOT(changingComponents(QCPBase*)));
    }
    xdoc.clear();
    errlog.flush();
    repaintTimer=startTimer(500);

    if (!errbuf.isEmpty()) {
        QLoadingLogDialog* dlg = new QLoadingLogDialog(this);
        dlg->setLogText(QString::fromUtf8(errbuf));
        dlg->exec();
        dlg->setParent(NULL);
        delete dlg;
    }
    errbuf.clear();
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
    QString s = QFileDialog::getOpenFileName(this,tr("Choose a file"),"","exSim files (*.exs)",
                                             NULL, QFileDialog::DontUseNativeDialog);
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
                                                     tr("exSim files (*.exs)"),NULL,
                                                     QFileDialog::DontUseNativeDialog);
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
                                                 tr("exSim files (*.exs)"),NULL,
                                                 QFileDialog::DontUseNativeDialog);
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
                       "(c) 2012-2013 by Kernel\n\n"
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
    a->move(100+ui->scrollArea->horizontalScrollBar()->value()+(qrand()%50-25),
            100+ui->scrollArea->verticalScrollBar()->value()+(qrand()%50-25));
    a->setObjectName(ac->whatsThis()+QString::number(renderArea->cpComponentCount(),10));
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

void MainWindow::editCut()
{
    editCopy();
    editDelete();
}

void MainWindow::editCopy()
{
    QDomDocument xdoc;
    QDomElement clist = xdoc.createElement("components");
    for(int i=0;i<renderArea->selection.count();i++) {
        QDomElement xc = xdoc.createElement(tr("element%1").arg(i));
        QCPBase* base = renderArea->selection.at(i);
        xc.setAttribute("className",base->metaObject()->className());
        xc.setAttribute("position",tr("%1,%2").arg(base->pos().x()).arg(base->pos().y()));
        xc.setAttribute("instanceName",base->objectName());
        base->storeToXML(xc);
        clist.appendChild(xc);
    }
    xdoc.appendChild(clist);
    QApplication::clipboard()->setText(xdoc.toString());
    xdoc.clear();
}

void MainWindow::cleanupBlock(QDomElement &block)
{
    int aidx = renderArea->cpComponentCount();
    QHash<QString,QString> rhash;
    rhash.clear();

    // rename new components to prevent collisions with schematic
    for (int i=0;i<block.childNodes().count();i++)
    {
        if (!block.childNodes().at(i).isElement()) continue;
        QDomElement xc = block.childNodes().at(i).toElement();
        QString newName = tr("%1%2").arg(xc.attribute("className","NULL")).arg(aidx+i);
        QString oldName = xc.attribute("instanceName","NULL");
        rhash[oldName]=newName;
        xc.setAttribute("instanceName",newName);
    }

    // remove external connections, update internal connections
    for (int i=0;i<block.childNodes().count();i++)
    {
        if (!block.childNodes().at(i).isElement()) continue;
        QDomElement xc = block.childNodes().at(i).toElement();

        QDomElement inp = xc.firstChildElement("inputs");
        QDomElement out = xc.firstChildElement("outputs");

        for (int i=0;i<inp.childNodes().count();i++)
        {
            if (!inp.childNodes().at(i).isElement()) continue;
            QDomElement xci = inp.childNodes().at(i).toElement();
            QString fromCmp = xci.attribute("fromCmp","NONE");
            if (!rhash.contains(fromCmp)) {
                xci.setAttribute("fromCmp","NONE");
                xci.setAttribute("fromPin","-1");
            } else
                xci.setAttribute("fromCmp",rhash[fromCmp]);
        }
        for (int i=0;i<out.childNodes().count();i++)
        {
            if (!out.childNodes().at(i).isElement()) continue;
            QDomElement xci = out.childNodes().at(i).toElement();
            QString toCmp = xci.attribute("toCmp","NONE");
            if (!rhash.contains(toCmp)) {
                xci.setAttribute("toCmp","NONE");
                xci.setAttribute("toPin","-1");
            } else
                xci.setAttribute("toCmp",rhash[toCmp]);
        }
    }
    rhash.clear();
}

void MainWindow::editPaste()
{
    QDomDocument xdoc;
    if (!xdoc.setContent(QApplication::clipboard()->text()))
        return;

    QByteArray errbuf;
    errbuf.clear();
    QTextStream errlog(&errbuf);

    QDomElement clist = xdoc.firstChildElement("components");
    cleanupBlock(clist);
    renderArea->readSchematic(errlog,clist,QPoint(50,50),true);

    for (int i=0;i<renderArea->selection.count();i++) {
        QCPBase* base = renderArea->selection.at(i);
        connect(base,SIGNAL(componentChanged(QCPBase*)),this,SLOT(changingComponents(QCPBase*)));
    }
    xdoc.clear();
    errlog.flush();
    if (!errbuf.isEmpty())
        qDebug() << errbuf;
    repaintTimer=startTimer(500);
}

void MainWindow::editDelete()
{
    for(int i=0;i<renderArea->selection.count();i++) {
        renderArea->selection[i]->deleteComponent();
    }
    renderArea->update();
    renderArea->repaintConn();
}
