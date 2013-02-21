#include "cprndgen.h"

QCPRndGen::QCPRndGen(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    state = 0;
    savedC = false;

    f1Out = new QCPOutput(this,this,"1"); fOutputs.append(f1Out);
    f2Out = new QCPOutput(this,this,"2"); fOutputs.append(f2Out);
    f4Out = new QCPOutput(this,this,"4"); fOutputs.append(f4Out);
    f8Out = new QCPOutput(this,this,"8"); fOutputs.append(f8Out);

    fCInp = new QCPInput(this,this,"C"); fInputs.append(fCInp);
    fRInp = new QCPInput(this,this,"R"); fInputs.append(fRInp);
}

QCPRndGen::~QCPRndGen()
{
    fInputs.clear();
    fOutputs.clear();
    delete f1Out;
    delete f2Out;
    delete f4Out;
    delete f8Out;
    delete fCInp;
    delete fRInp;
}

QSize QCPRndGen::minimumSizeHint() const
{
    return QSize(100*zoom()/100,getDCompHeight(0));
}

void QCPRndGen::realignPins(QPainter &)
{
    int dy = getDCompIncrement();
    int ddy = dy/2;
    f1Out->relCoord = QPoint(width()-getPinSize()/2,dy);
    f2Out->relCoord = QPoint(width()-getPinSize()/2,2*dy);
    f4Out->relCoord = QPoint(width()-getPinSize()/2,3*dy);
    f8Out->relCoord = QPoint(width()-getPinSize()/2,4*dy);
    fCInp->relCoord = QPoint(getPinSize()/2,dy+ddy);
    fRInp->relCoord = QPoint(getPinSize()/2,3*dy+ddy);
}

void QCPRndGen::doLogicPrivate()
{
    bool C = fCInp->state;
    bool R = fRInp->state;

    if (R) state = 0;
    else if (!C && savedC) {
        state = qrand() & 0xf;
    }

    savedC = C;

    f1Out->state = ((state & 0x1) > 0);
    f2Out->state = ((state & 0x2) > 0);
    f4Out->state = ((state & 0x4) > 0);
    f8Out->state = ((state & 0x8) > 0);
}

void QCPRndGen::paintEvent(QPaintEvent *)
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
    rc.adjust(p.fontMetrics().width("OE")+getPinSize()/2,0,-1,-1);
    p.drawLine(rc.topLeft(),rc.bottomLeft());

    rc = rect();
    rc.adjust(width()-p.fontMetrics().width("OE")-getPinSize()/2,0,-1,-1);
    p.drawLine(rc.topLeft(),rc.bottomLeft());

    rc = rect();
    rc.adjust(0,0,-1,0);
    rc.setHeight(getDCompIncrement()*3);
    n.setBold(false);
    p.setFont(n);
    p.drawText(rc,Qt::AlignCenter,"RND");

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);
}
