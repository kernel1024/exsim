#include "cpcounter.h"

QCPCounter::QCPCounter(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    state = 0;
    savedC = false;
    savedInc = false;
    savedDec = false;

    f1Out = new QCPOutput(this,this,"1"); fOutputs.append(f1Out);
    f2Out = new QCPOutput(this,this,"2"); fOutputs.append(f2Out);
    f4Out = new QCPOutput(this,this,"4"); fOutputs.append(f4Out);
    f8Out = new QCPOutput(this,this,"8"); fOutputs.append(f8Out);
    fUFOut = new QCPOutput(this,this,QChar(0x2264)); fOutputs.append(fUFOut);
    fOFOut = new QCPOutput(this,this,QChar(0x2265)); fOutputs.append(fOFOut);

    f1Inp = new QCPInput(this,this,"1"); fInputs.append(f1Inp);
    f2Inp = new QCPInput(this,this,"2"); fInputs.append(f2Inp);
    f4Inp = new QCPInput(this,this,"4"); fInputs.append(f4Inp);
    f8Inp = new QCPInput(this,this,"8"); fInputs.append(f8Inp);
    fCInp = new QCPInput(this,this,"C"); fInputs.append(fCInp);
    fRInp = new QCPInput(this,this,"R"); fInputs.append(fRInp);
    fIncInp = new QCPInput(this,this,"+1"); fInputs.append(fIncInp);
    fDecInp = new QCPInput(this,this,"-1"); fInputs.append(fDecInp);
    fModeInp = new QCPInput(this,this,"DM"); fInputs.append(fModeInp);
}

QCPCounter::~QCPCounter()
{
    fInputs.clear();
    fOutputs.clear();
    delete f1Out;
    delete f2Out;
    delete f4Out;
    delete f8Out;
    delete fUFOut;
    delete fOFOut;
    delete f1Inp;
    delete f2Inp;
    delete f4Inp;
    delete f8Inp;
    delete fCInp;
    delete fRInp;
    delete fIncInp;
    delete fDecInp;
    delete fModeInp;
}

QSize QCPCounter::minimumSizeHint() const
{
    return QSize(100*zoom()/100,getDCompHeight(1));
}

void QCPCounter::realignPins(QPainter &)
{
    int dy = getDCompIncrement();
    int ddy = dy/2;
    f1Inp->relCoord = QPoint(getPinSize()/2,dy);
    f1Out->relCoord = QPoint(width()-getPinSize()/2,dy);
    f2Inp->relCoord = QPoint(getPinSize()/2,2*dy);
    f2Out->relCoord = QPoint(width()-getPinSize()/2,2*dy);
    f4Inp->relCoord = QPoint(getPinSize()/2,3*dy);
    f4Out->relCoord = QPoint(width()-getPinSize()/2,3*dy);
    f8Inp->relCoord = QPoint(getPinSize()/2,4*dy);
    f8Out->relCoord = QPoint(width()-getPinSize()/2,4*dy);
    fCInp->relCoord = QPoint(getPinSize()/2,5*dy+ddy);
    fRInp->relCoord = QPoint(getPinSize()/2,6*dy+ddy);
    fIncInp->relCoord = QPoint(getPinSize()/2,7*dy+ddy);
    fDecInp->relCoord = QPoint(getPinSize()/2,8*dy+ddy);
    fOFOut->relCoord = QPoint(width()-getPinSize()/2,8*dy+ddy);
    fModeInp->relCoord = QPoint(getPinSize()/2,9*dy+ddy);
    fUFOut->relCoord = QPoint(width()-getPinSize()/2,9*dy+ddy);
}

void QCPCounter::doLogicPrivate()
{
    bool inc = fIncInp->state;
    bool dec = fDecInp->state;
    bool C = fCInp->state;
    bool R = fRInp->state;
    bool mode = fModeInp->state;
    int max = 15;
    if (mode)
        max = 9;
    qint8 di = ((f8Inp->state << 3) & 0x8) |
               ((f4Inp->state << 2) & 0x4) |
               ((f2Inp->state << 1) & 0x2) |
               (f1Inp->state & 0x1);

    if (R) state = 0;
    else if (!C && savedC) {
        state = di;
    } else if (!inc && savedInc) {
        state++;
    } else if (!dec && savedDec) {
        state--;
    }

    if (state>max) state=0;
    if (state<0) state=max;

    savedC = C;
    savedInc = inc;
    savedDec = dec;

    f1Out->state = ((state & 0x1) > 0);
    f2Out->state = ((state & 0x2) > 0);
    f4Out->state = ((state & 0x4) > 0);
    f8Out->state = ((state & 0x8) > 0);
    fUFOut->state = ((state == 0) && dec);
    fOFOut->state = ((state == max) && inc);
}

void QCPCounter::paintEvent(QPaintEvent *)
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

    int dy = getDCompIncrement();
    int ddy = dy/2;

    rc = rect();
    rc.adjust(p.fontMetrics().width("DM")+getPinSize()/2,0,-1,-1);
    p.drawLine(rc.topLeft(),rc.bottomLeft());

    p.drawLine(0,4*dy+ddy+getPinSize()/2,rc.left(),4*dy+ddy+getPinSize()/2);

    rc = rect();
    rc.adjust(width()-p.fontMetrics().width("DM")-getPinSize()/2,0,-1,-1);
    p.drawLine(rc.topLeft(),rc.bottomLeft());

    p.drawLine(rc.left(),7*dy+ddy+getPinSize()/2,rc.right(),7*dy+ddy+getPinSize()/2);

    rc = rect();
    rc.adjust(0,0,-1,0);
    rc.setHeight(getDCompIncrement()*3);
    n.setBold(false);
    p.setFont(n);
    p.drawText(rc,Qt::AlignCenter,"CT");

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);
}
