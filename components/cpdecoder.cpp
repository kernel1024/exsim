#include "cpdecoder.h"

QCPDecoder::QCPDecoder(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    state = 0;

    f1Out = new QCPOutput(this,this,"0"); fOutputs.append(f1Out);
    f2Out = new QCPOutput(this,this,"1"); fOutputs.append(f2Out);
    f3Out = new QCPOutput(this,this,"2"); fOutputs.append(f3Out);
    f4Out = new QCPOutput(this,this,"3"); fOutputs.append(f4Out);
    f5Out = new QCPOutput(this,this,"4"); fOutputs.append(f5Out);
    f6Out = new QCPOutput(this,this,"5"); fOutputs.append(f6Out);
    f7Out = new QCPOutput(this,this,"6"); fOutputs.append(f7Out);
    f8Out = new QCPOutput(this,this,"7"); fOutputs.append(f8Out);

    f1Inp = new QCPInput(this,this,"1");  fInputs.append(f1Inp);
    f2Inp = new QCPInput(this,this,"2");  fInputs.append(f2Inp);
    f4Inp = new QCPInput(this,this,"4");  fInputs.append(f4Inp);
    fOEInp = new QCPInput(this,this,"OE");fInputs.append(fOEInp);
}

QCPDecoder::~QCPDecoder()
{
    fInputs.clear();
    fOutputs.clear();
    delete f1Out;
    delete f2Out;
    delete f3Out;
    delete f4Out;
    delete f5Out;
    delete f6Out;
    delete f7Out;
    delete f8Out;
    delete f1Inp;
    delete f2Inp;
    delete f4Inp;
    delete fOEInp;
}

QSize QCPDecoder::minimumSizeHint() const
{
    return QSize(100*zoom()/100,getDCompHeight(0));
}

void QCPDecoder::realignPins(QPainter &)
{
    int dy = getDCompIncrement();
    f1Inp->relCoord = QPoint(getPinSize()/2,dy);
    f1Out->relCoord = QPoint(width()-getPinSize()/2,dy);
    f2Inp->relCoord = QPoint(getPinSize()/2,2*dy);
    f2Out->relCoord = QPoint(width()-getPinSize()/2,2*dy);
    f4Inp->relCoord = QPoint(getPinSize()/2,3*dy);
    f3Out->relCoord = QPoint(width()-getPinSize()/2,3*dy);
    f4Out->relCoord = QPoint(width()-getPinSize()/2,4*dy);
    f5Out->relCoord = QPoint(width()-getPinSize()/2,5*dy);
    f6Out->relCoord = QPoint(width()-getPinSize()/2,6*dy);
    f7Out->relCoord = QPoint(width()-getPinSize()/2,7*dy);
    f8Out->relCoord = QPoint(width()-getPinSize()/2,8*dy);
    fOEInp->relCoord = QPoint(getPinSize()/2,8*dy);
}

void QCPDecoder::doLogicPrivate()
{
    bool OE = fOEInp->state;
    qint8 di = ((f4Inp->state << 2) & 0x4) |
               ((f2Inp->state << 1) & 0x2) |
               (f1Inp->state & 0x1);

    if (!OE) state = 0;
    else
        state = di;

    for(int i=0;i<fOutputs.count();i++)
        fOutputs[i]->state = (i==state);
}

void QCPDecoder::paintEvent(QPaintEvent *)
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

    int dy = getDCompIncrement();
    p.drawLine(0,7*dy+getPinSize()/2,rc.left(),7*dy+getPinSize()/2);

    rc = rect();
    rc.adjust(width()-p.fontMetrics().width("OE")-getPinSize()/2,0,-1,-1);
    p.drawLine(rc.topLeft(),rc.bottomLeft());

    rc = rect();
    rc.adjust(0,0,-1,0);
    rc.setHeight(getDCompIncrement()*3);
    n.setBold(false);
    p.setFont(n);
    p.drawText(rc,Qt::AlignCenter,"DC");

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);
}
