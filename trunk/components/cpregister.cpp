#include "cpregister.h"

QCPRegister::QCPRegister(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    state = 0;
    savedC = false;
    savedLS = false;
    savedRS = false;

    f1Out = new QCPOutput(this,this,"1"); fOutputs.append(f1Out);
    f2Out = new QCPOutput(this,this,"2"); fOutputs.append(f2Out);
    f4Out = new QCPOutput(this,this,"4"); fOutputs.append(f4Out);
    f8Out = new QCPOutput(this,this,"8"); fOutputs.append(f8Out);

    f1Inp = new QCPInput(this,this,"1"); fInputs.append(f1Inp);
    f2Inp = new QCPInput(this,this,"2"); fInputs.append(f2Inp);
    f4Inp = new QCPInput(this,this,"4"); fInputs.append(f4Inp);
    f8Inp = new QCPInput(this,this,"8"); fInputs.append(f8Inp);
    fCInp = new QCPInput(this,this,"C"); fInputs.append(fCInp);
    fRInp = new QCPInput(this,this,"R"); fInputs.append(fRInp);
    fLSInp = new QCPInput(this,this,"<-"); fInputs.append(fLSInp);
    fRSInp = new QCPInput(this,this,"->"); fInputs.append(fRSInp);
}

QCPRegister::~QCPRegister()
{
    fInputs.clear();
    fOutputs.clear();
    delete f1Out;
    delete f2Out;
    delete f4Out;
    delete f8Out;
    delete f1Inp;
    delete f2Inp;
    delete f4Inp;
    delete f8Inp;
    delete fCInp;
    delete fRInp;
    delete fLSInp;
    delete fRSInp;
}

QSize QCPRegister::minimumSizeHint() const
{
    return QSize(100*zoom()/100,getDCompHeight(1));
}

void QCPRegister::realignPins(QPainter &)
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
    fLSInp->relCoord = QPoint(getPinSize()/2,5*dy+ddy);
    fRSInp->relCoord= QPoint(getPinSize()/2,6*dy+ddy);
    fCInp->relCoord = QPoint(getPinSize()/2,7*dy+ddy);
    fRInp->relCoord = QPoint(getPinSize()/2,8*dy+ddy);
}

void QCPRegister::doLogicPrivate()
{
    bool LS = fLSInp->state;
    bool RS = fRSInp->state;
    bool C = fCInp->state;
    bool R = fRInp->state;
    qint8 di = ((f8Inp->state << 3) & 0x8) |
               ((f4Inp->state << 2) & 0x4) |
               ((f2Inp->state << 1) & 0x2) |
               (f1Inp->state & 0x1);

    if (R) state = 0;
    else if (!C && savedC) {
        state = di;
    } else if (!LS && savedLS) {
        state = (state << 1) & 0xf;
    } else if (!RS && savedRS) {
        state = (state >> 1) & 0xf;
    }

    savedC = C;
    savedLS = LS;
    savedRS = RS;

    f1Out->state = ((state & 0x1) > 0);
    f2Out->state = ((state & 0x2) > 0);
    f4Out->state = ((state & 0x4) > 0);
    f8Out->state = ((state & 0x8) > 0);
}

void QCPRegister::paintEvent(QPaintEvent *)
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
    rc.adjust(p.fontMetrics().width("->")+getPinSize()/2,0,-1,-1);
    p.drawLine(rc.topLeft(),rc.bottomLeft());

    int dy = getDCompIncrement();
    int ddy = dy/2;
    p.drawLine(0,4*dy+ddy+getPinSize()/2,rc.left(),4*dy+ddy+getPinSize()/2);

    rc = rect();
    rc.adjust(width()-p.fontMetrics().width("->")-getPinSize()/2,0,-1,-1);
    p.drawLine(rc.topLeft(),rc.bottomLeft());

    rc = rect();
    rc.adjust(0,0,-1,0);
    rc.setHeight(getDCompIncrement()*3);
    n.setBold(false);
    p.setFont(n);
    p.drawText(rc,Qt::AlignCenter,"RG");

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);

    drawSelection(p);
}
