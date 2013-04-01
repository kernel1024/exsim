#include "cpextender.h"
#include "renderarea.h"
#include <QInputDialog>

QCPExtender::QCPExtender(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    fInp = new QCPInput(this,this);
    fInputs.append(fInp);
    fOutputs << new QCPOutput(this,this);
    fOutputs << new QCPOutput(this,this);
    fOutputs << new QCPOutput(this,this);
}

QCPExtender::~QCPExtender()
{
    for(int i=0;i<fOutputs.count();i++)
        fOutputs.at(i)->deleteLater();
    fInputs.clear();
    fOutputs.clear();
    delete fInp;
}

QSize QCPExtender::minimumSizeHint() const
{
    int vsz = (QApplication::fontMetrics().height()+4)*(fOutputs.count()+1);
    if (vsz<40) vsz = 40;
    return QSize(40*zoom()/100,vsz*zoom()/100);
}

void QCPExtender::readFromXML(QTextStream &errlog, const QDomElement &element)
{
    bool ok;
    int outcnt = element.attribute("outCount","3").toInt(&ok);
    if ((!ok) || (outcnt<1) || (outcnt>32)) {
        errlog << tr("QCPExtender: outCount value incorrect") << endl;
        outcnt = 3;
    }
    setMode(outcnt);
    QCPBase::readFromXML(errlog,element);
}

void QCPExtender::storeToXML(QDomElement &element)
{
    element.setAttribute("outCount",fOutputs.count());
    QCPBase::storeToXML(element);
}

void QCPExtender::setMode(int outputCount)
{
    int icnt = outputCount;
    if (icnt>32)
        icnt=32;
    if (icnt<2)
        icnt=2;

    deleteOutputs();
    cpOwner->repaintConn();

    // create new outputs
    for(int i=0;i<icnt;i++)
        fOutputs << new QCPOutput(this,this);

    savedState=QString();
    doLogic();

    resize(minimumSizeHint());
    update();
}

void QCPExtender::realignPins()
{
    fInp->relCoord=QPoint(getPinSize()/2,height()/2);
    int dy = height()/(fOutputs.count()+1);
    for(int i=0;i<fOutputs.count();i++)
        fOutputs[i]->relCoord=QPoint(width()-getPinSize()/2,i*dy+dy);
}

void QCPExtender::doLogicPrivate()
{
    for(int i=0;i<fOutputs.count();i++)
        fOutputs.at(i)->state=fInp->state;
}

void QCPExtender::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QPen op=p.pen();
    QBrush ob=p.brush();
    QFont of=p.font();

    p.setPen(QPen(Qt::black));
    p.setBrush(QBrush(Qt::white,Qt::SolidPattern));
    QRect rc = rect();
    rc.adjust(0,0,-1,-1);
    p.drawRect(rc);

    int y = fInp->relCoord.y();
    int px = rc.width()/2;
    QPoint cp = QPoint(px,y);
    p.drawLine(fInp->relCoord,cp);
    for(int i=0;i<fOutputs.count();i++) {
        QCPOutput* fOut = fOutputs.at(i);
        QPoint lp = QPoint(px,fOut->relCoord.y());
        p.drawLine(cp,lp);
        p.drawLine(lp,fOut->relCoord);
    }

    redrawPins(p);

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);

    drawSelection(p);
}

void QCPExtender::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu cm(this);
    cm.addAction(tr("Change outputs count..."),this,SLOT(adjustOutCount()));
    cm.exec(event->globalPos());
}

void QCPExtender::adjustOutCount()
{
    bool ok;
    int cnt = QInputDialog::getInt(this,tr("Extender outputs count"),tr("Count"),fOutputs.count(),2,32,1,&ok);
    if (ok)
        setMode(cnt);
}
