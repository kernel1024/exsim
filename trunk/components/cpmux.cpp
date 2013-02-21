#include "cpmux.h"

QCPMux::QCPMux(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    fXOut = new QCPOutput(this,this,"X"); fOutputs.append(fXOut);
    fYOut = new QCPOutput(this,this,"Y"); fOutputs.append(fYOut);

    fX1Inp = new QCPInput(this,this,"X0"); fInputs.append(fX1Inp);
    fX2Inp = new QCPInput(this,this,"X1"); fInputs.append(fX2Inp);
    fX3Inp = new QCPInput(this,this,"X2"); fInputs.append(fX3Inp);
    fX4Inp = new QCPInput(this,this,"X3"); fInputs.append(fX4Inp);
    fY1Inp = new QCPInput(this,this,"Y0"); fInputs.append(fY1Inp);
    fY2Inp = new QCPInput(this,this,"Y1"); fInputs.append(fY2Inp);
    fY3Inp = new QCPInput(this,this,"Y2"); fInputs.append(fY3Inp);
    fY4Inp = new QCPInput(this,this,"Y3"); fInputs.append(fY4Inp);
    fA0Inp = new QCPInput(this,this,"A0"); fInputs.append(fA0Inp);
    fA1Inp = new QCPInput(this,this,"A1"); fInputs.append(fA1Inp);
}

QCPMux::~QCPMux()
{
    fInputs.clear();
    fOutputs.clear();
    delete fXOut;
    delete fYOut;
    delete fX1Inp;
    delete fX2Inp;
    delete fX3Inp;
    delete fX4Inp;
    delete fY1Inp;
    delete fY2Inp;
    delete fY3Inp;
    delete fY4Inp;
    delete fA0Inp;
    delete fA1Inp;
}

QSize QCPMux::minimumSizeHint() const
{
    return QSize(100*zoom()/100,getDCompHeight(2));
}

void QCPMux::realignPins(QPainter &)
{
    int dy = getDCompIncrement();
    int ddy = dy/2;
    fX1Inp->relCoord = QPoint(getPinSize()/2,dy);
    fX2Inp->relCoord = QPoint(getPinSize()/2,2*dy);
    fX3Inp->relCoord = QPoint(getPinSize()/2,3*dy);
    fX4Inp->relCoord = QPoint(getPinSize()/2,4*dy);
    fY1Inp->relCoord = QPoint(getPinSize()/2,5*dy+ddy);
    fY2Inp->relCoord = QPoint(getPinSize()/2,6*dy+ddy);
    fY3Inp->relCoord = QPoint(getPinSize()/2,7*dy+ddy);
    fY4Inp->relCoord = QPoint(getPinSize()/2,8*dy+ddy);
    fA0Inp->relCoord = QPoint(getPinSize()/2,9*dy+2*ddy);
    fA1Inp->relCoord = QPoint(getPinSize()/2,10*dy+2*ddy);

    fXOut->relCoord = QPoint(width()-getPinSize()/2,10*dy/4);
    fYOut->relCoord = QPoint(width()-getPinSize()/2,(26*dy+4*ddy)/4);
}

void QCPMux::doLogicPrivate()
{
    qint8 di = ((fA1Inp->state << 1) & 0x2) |
                (fA0Inp->state & 0x1);

    fXOut->state = fInputs[di]->state;
    fYOut->state = fInputs[di+4]->state;
}

void QCPMux::paintEvent(QPaintEvent *)
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

    QFont n=QApplication::font();
    p.setPen(QPen(Qt::black));
    n.setPointSize((n.pointSize()+4)*zoom()/100);
    n.setBold(true);
    p.setFont(n);

    rc = rect();
    rc.adjust(p.fontMetrics().width("A0")+getPinSize()/2,0,-1,-1);
    p.drawLine(rc.topLeft(),rc.bottomLeft());

    int dy = getDCompIncrement();
    int ddy = dy/2;
    p.drawLine(0,4*dy+ddy+getPinSize()/2,rc.left(),4*dy+ddy+getPinSize()/2);
    p.drawLine(0,8*dy+2*ddy+getPinSize()/2,rc.left(),8*dy+2*ddy+getPinSize()/2);

    rc = rect();
    rc.adjust(width()-p.fontMetrics().width("A0")-getPinSize()/2,0,-1,-1);
    p.drawLine(rc.topLeft(),rc.bottomLeft());

    rc = rect();
    rc.adjust(0,0,-1,0);
    rc.setHeight(getDCompIncrement()*3);
    n.setBold(false);
    p.setFont(n);
    p.drawText(rc,Qt::AlignCenter,"MX");

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);
}
