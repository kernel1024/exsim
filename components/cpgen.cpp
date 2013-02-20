#include "cpgen.h"
#include "renderarea.h"

QCPGen::QCPGen(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    fOut = new QCPOutput(this,this);
    fOut->pinName = "Q";
    fOutputs.append(fOut);
    genState = false;
    connect(&mainTimer,SIGNAL(timeout()),this,SLOT(timeImpulse()));
    setPeriod(2000);
}

QCPGen::~QCPGen()
{
    fOutputs.clear();
    delete fOut;
}

QSize QCPGen::minimumSizeHint() const
{
    return QSize(4*QApplication::fontMetrics().width("OSC") * zoom()/100,
                 2*QApplication::fontMetrics().height()     * zoom()/100);
}

void QCPGen::readFromStream(QDataStream &stream)
{
    stream >> period;
    setPeriod(period);
    QCPBase::readFromStream(stream);
}

void QCPGen::storeToStream(QDataStream &stream)
{
    stream << period;
    QCPBase::storeToStream(stream);
}

void QCPGen::realignPins(QPainter &)
{
    fOut->relCoord=QPoint(width()-getPinSize()/2,height()/2);
}

void QCPGen::doLogicPrivate()
{
    fOut->state = genState;
}

void QCPGen::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu cm(this);
    cm.addAction(tr("Change period..."),this,SLOT(adjustPeriod()));
    cm.exec(event->globalPos());
}

void QCPGen::adjustPeriod()
{
    bool ok;
    int cnt = QInputDialog::getInt(this,tr("Generation period"),tr("Milliseconds"),period,20,60000,10,&ok);
    if (ok)
        setPeriod(cnt);
}

void QCPGen::paintEvent(QPaintEvent *)
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

    redrawPins(p);

    rc.adjust(0,0,-p.fontMetrics().width("Q")-getPinSize(),0);
    QFont n=QApplication::font();
    n.setPointSize((n.pointSize()) * zoom()/100);
    p.setFont(n);
    p.drawText(rc,Qt::AlignCenter,"Oscillator");

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);
}


void QCPGen::setPeriod(int msec)
{
    period = msec;
    mainTimer.setInterval(period/2);
    mainTimer.start();
}


void QCPGen::timeImpulse()
{
    if (isHidden()) return;
    genState = !genState;
    doLogic();
}