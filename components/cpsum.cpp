#include "cpsum.h"

QCPSum::QCPSum(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    state = 0;
    f1Out = new QCPOutput(this,this,"1"); fOutputs.append(f1Out);
    f2Out = new QCPOutput(this,this,"2"); fOutputs.append(f2Out);
    f4Out = new QCPOutput(this,this,"4"); fOutputs.append(f4Out);
    f8Out = new QCPOutput(this,this,"8"); fOutputs.append(f8Out);
    fOFOut = new QCPOutput(this,this,QChar(0x2265)); fOutputs.append(fOFOut);

    fX1Inp = new QCPInput(this,this,"X0"); fInputs.append(fX1Inp);
    fX2Inp = new QCPInput(this,this,"X1"); fInputs.append(fX2Inp);
    fX4Inp = new QCPInput(this,this,"X2"); fInputs.append(fX4Inp);
    fX8Inp = new QCPInput(this,this,"X3"); fInputs.append(fX8Inp);
    fY1Inp = new QCPInput(this,this,"Y0"); fInputs.append(fY1Inp);
    fY2Inp = new QCPInput(this,this,"Y1"); fInputs.append(fY2Inp);
    fY4Inp = new QCPInput(this,this,"Y2"); fInputs.append(fY4Inp);
    fY8Inp = new QCPInput(this,this,"Y3"); fInputs.append(fY8Inp);
    fOFInp = new QCPInput(this,this,QChar(0x2265)); fInputs.append(fOFInp);
}

QCPSum::~QCPSum()
{
    fInputs.clear();
    fOutputs.clear();
    delete f1Out;
    delete f2Out;
    delete f4Out;
    delete f8Out;
    delete fOFOut;
    delete fX1Inp;
    delete fX2Inp;
    delete fX4Inp;
    delete fX8Inp;
    delete fY1Inp;
    delete fY2Inp;
    delete fY4Inp;
    delete fY8Inp;
    delete fOFInp;
}

QSize QCPSum::minimumSizeHint() const
{
    return QSize(100*zoom()/100,getDCompHeight(2));
}

void QCPSum::realignPins(QPainter &)
{
    int dy = getDCompIncrement();
    int ddy = dy/2;
    fX1Inp->relCoord = QPoint(getPinSize()/2,dy);
    fX2Inp->relCoord = QPoint(getPinSize()/2,2*dy);
    fX4Inp->relCoord = QPoint(getPinSize()/2,3*dy);
    fX8Inp->relCoord = QPoint(getPinSize()/2,4*dy);
    fY1Inp->relCoord = QPoint(getPinSize()/2,5*dy+ddy);
    fY2Inp->relCoord = QPoint(getPinSize()/2,6*dy+ddy);
    fY4Inp->relCoord = QPoint(getPinSize()/2,7*dy+ddy);
    fY8Inp->relCoord = QPoint(getPinSize()/2,8*dy+ddy);
    fOFInp->relCoord = QPoint(getPinSize()/2,9*dy+2*ddy);

    f1Out->relCoord = QPoint(width()-getPinSize()/2,3*dy);
    f2Out->relCoord = QPoint(width()-getPinSize()/2,4*dy);
    f4Out->relCoord = QPoint(width()-getPinSize()/2,5*dy);
    f8Out->relCoord = QPoint(width()-getPinSize()/2,6*dy);
    fOFOut->relCoord = QPoint(width()-getPinSize()/2,9*dy+2*ddy);
}

void QCPSum::doLogicPrivate()
{
    quint8 x = ((fX8Inp->state << 3) & 0x8) |
               ((fX4Inp->state << 2) & 0x4) |
               ((fX2Inp->state << 1) & 0x2) |
               (fX1Inp->state & 0x1);
    quint8 y = ((fY8Inp->state << 3) & 0x8) |
               ((fY4Inp->state << 2) & 0x4) |
               ((fY2Inp->state << 1) & 0x2) |
               (fY1Inp->state & 0x1);
    quint8 p = (fOFInp->state & 0x1);

    state = x+y+p;

    f1Out->state = ((state & 0x1) > 0);
    f2Out->state = ((state & 0x2) > 0);
    f4Out->state = ((state & 0x4) > 0);
    f8Out->state = ((state & 0x8) > 0);
    fOFOut->state = ((state & 0x10) > 0);
}

void QCPSum::paintEvent(QPaintEvent *)
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
    p.drawLine(rc.left(),8*dy+2*ddy+getPinSize()/2,rc.right(),8*dy+2*ddy+getPinSize()/2);

    rc = rect();
    rc.adjust(0,0,-1,0);
    rc.setHeight(getDCompIncrement()*3);
    n.setBold(false);
    p.setFont(n);
    p.drawText(rc,Qt::AlignCenter,"SM");

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);

    drawSelection(p);
}
