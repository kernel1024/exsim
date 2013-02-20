#include "cpencoder.h"

QCPEncoder::QCPEncoder(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    state = 0;

    f1Out = new QCPOutput(this,this,"1"); fOutputs.append(f1Out);
    f2Out = new QCPOutput(this,this,"2"); fOutputs.append(f2Out);
    f4Out = new QCPOutput(this,this,"4"); fOutputs.append(f4Out);

    f1Inp = new QCPInput(this,this,"1");  fInputs.append(f1Inp);
    f2Inp = new QCPInput(this,this,"2");  fInputs.append(f2Inp);
    f3Inp = new QCPInput(this,this,"3");  fInputs.append(f3Inp);
    f4Inp = new QCPInput(this,this,"4");  fInputs.append(f4Inp);
    f5Inp = new QCPInput(this,this,"5");  fInputs.append(f5Inp);
    f6Inp = new QCPInput(this,this,"6");  fInputs.append(f6Inp);
    f7Inp = new QCPInput(this,this,"7");  fInputs.append(f7Inp);
}

QCPEncoder::~QCPEncoder()
{
    fInputs.clear();
    fOutputs.clear();
    delete f1Out;
    delete f2Out;
    delete f4Out;
    delete f1Inp;
    delete f2Inp;
    delete f3Inp;
    delete f4Inp;
    delete f5Inp;
    delete f6Inp;
    delete f7Inp;
}

QSize QCPEncoder::minimumSizeHint() const
{
    return QSize(100*zoom()/100,getDCompHeight(0)*zoom()/100);
}

void QCPEncoder::realignPins(QPainter &)
{
    int dy = getDCompIncrement();
    f1Inp->relCoord = QPoint(getPinSize()/2,dy);
    f1Out->relCoord = QPoint(width()-getPinSize()/2,dy);
    f2Inp->relCoord = QPoint(getPinSize()/2,2*dy);
    f2Out->relCoord = QPoint(width()-getPinSize()/2,2*dy);
    f3Inp->relCoord = QPoint(getPinSize()/2,3*dy);
    f4Out->relCoord = QPoint(width()-getPinSize()/2,3*dy);
    f4Inp->relCoord = QPoint(getPinSize()/2,4*dy);
    f5Inp->relCoord = QPoint(getPinSize()/2,5*dy);
    f6Inp->relCoord = QPoint(getPinSize()/2,6*dy);
    f7Inp->relCoord = QPoint(getPinSize()/2,7*dy);
}

void QCPEncoder::doLogicPrivate()
{
    qint8 di = -1;
    for(int i=fInputs.count()-1;i>=0;i--)
        if (fInputs.at(i)->state) {
            di = i;
            break;
        }
    state = di+1;

    f1Out->state = ((state & 0x1) > 0);
    f2Out->state = ((state & 0x2) > 0);
    f4Out->state = ((state & 0x4) > 0);
}

void QCPEncoder::paintEvent(QPaintEvent *)
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
    p.drawText(rc,Qt::AlignCenter,"CD");

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);
}
