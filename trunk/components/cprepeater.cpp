#include "cprepeater.h"

QCPRepeater::QCPRepeater(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    fInp = new QCPInput(this,this);
    fInp->pinName = "T";
    fInputs.append(fInp);
    fOut = new QCPOutput(this,this);
    fOut->pinName =  "Q";
    fOutputs.append(fOut);
    connect(this,SIGNAL(sendDelayedUpdate(bool)),this,SLOT(delayedUpdate(bool)),Qt::QueuedConnection);
}

QCPRepeater::~QCPRepeater()
{
    fOutputs.clear();
    delete fOut;
    fInputs.clear();
    delete fInp;
}

QSize QCPRepeater::minimumSizeHint() const
{
    return QSize(4*QApplication::fontMetrics().width("OSC") * zoom()/100,
                 2*QApplication::fontMetrics().height()     * zoom()/100);
}

void QCPRepeater::realignPins(QPainter &)
{
    fInp->relCoord=QPoint(getPinSize()/2,height()/2);
    fOut->relCoord=QPoint(width()-getPinSize()/2,height()/2);
}

void QCPRepeater::doLogicPrivate()
{
    emit sendDelayedUpdate(fInp->state);
}

void QCPRepeater::paintEvent(QPaintEvent *)
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

    rc.adjust(p.fontMetrics().width("Q")+getPinSize(),0,-p.fontMetrics().width("Q")-getPinSize(),0);
    QFont n=QApplication::font();
    n.setPointSize((n.pointSize()) * zoom()/100);
    p.setFont(n);
    p.drawText(rc,Qt::AlignCenter,"Repeater");

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);
}

void QCPRepeater::delayedUpdate(bool state)
{
    if (isHidden()) return;
    fOut->state = state;
    fOut->applyState();
}
