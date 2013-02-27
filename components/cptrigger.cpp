#include "cptrigger.h"

QCPTrigger::QCPTrigger(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    state = false;
    mode = TM_RSD;
    savedT = false;
    fQOut = new QCPOutput(this,this,"Q");
    fOutputs.append(fQOut);
    fNQOut = new QCPOutput(this,this,"Q");
    fNQOut->inversed = true;
    fNQOut->state = true;
    fOutputs.append(fNQOut);
    fSInp = new QCPInput(this,this,"S");
    fInputs.append(fSInp);
    fDTInp = new QCPInput(this,this,"D");
    fInputs.append(fDTInp);
    fCInp = new QCPInput(this,this,"C");
    fInputs.append(fCInp);
    fRInp = new QCPInput(this,this,"R");
    fInputs.append(fRInp);
}

QCPTrigger::~QCPTrigger()
{
    fInputs.clear();
    fOutputs.clear();
    delete fQOut;
    delete fNQOut;
    delete fSInp;
    delete fDTInp;
    delete fCInp;
    delete fRInp;
}

QSize QCPTrigger::minimumSizeHint() const
{
    int vsz = (QApplication::fontMetrics().height()+4)*7;
    if (vsz<80) vsz = 80;
    return QSize(80*zoom()/100,vsz*zoom()/100);
}

void QCPTrigger::readFromXML(QTextStream &errlog, const QDomElement &element)
{
    bool ok;
    int lt = element.attribute("mode","1").toInt(&ok);
    if ((!ok) || (lt<1) || (lt>2)) {
        errlog << tr("QCPTrigger: mode value incorrect") << endl;
        lt = 1;
    }
    setMode((TriggerMode)lt);
    QCPBase::readFromXML(errlog,element);
}

void QCPTrigger::storeToXML(QDomElement &element)
{
    element.setAttribute("mode",(int)mode);
    QCPBase::storeToXML(element);
}

void QCPTrigger::setMode(QCPTrigger::TriggerMode tMode)
{
    mode = tMode;

    if (mode==TM_RSD) {
        fDTInp->pinName="D";
        fCInp->pinName="C";
        savedT = fDTInp->state;
    } else {
        fDTInp->pinName="T";
        fCInp->pinName=" ";

    }

    resize(minimumSizeHint());
    update();
}

void QCPTrigger::realignPins(QPainter &)
{
    int dy = height()/6;
    int ddy = dy/2;
    fSInp->relCoord = QPoint(getPinSize()/2,dy);
    fQOut->relCoord = QPoint(width()-getPinSize()/2,dy);
    fDTInp->relCoord = QPoint(getPinSize()/2,2*dy+ddy);
    fCInp->relCoord = QPoint(getPinSize()/2,3*dy+ddy);
    fRInp->relCoord = QPoint(getPinSize()/2,4*dy+2*ddy);
    fNQOut->relCoord= QPoint(width()-getPinSize()/2,4*dy+2*ddy);
}

void QCPTrigger::doLogicPrivate()
{
    bool S = fSInp->state;
    bool DT = fDTInp->state;
    bool C = fCInp->state;
    bool R = fRInp->state;

    if (!(S && R)) {
        if (S) state = true;
        if (R) state = false;
    }
    if (mode==TM_RSD) {
        if (!S && !R && C)
            state = DT;
    } else {
        if (!DT && savedT)
            state = !state;
        savedT = DT;
    }

    fQOut->state = state;
    fNQOut->state = !state;
}

void QCPTrigger::paintEvent(QPaintEvent *)
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
    n.setBold(true);
    n.setPointSize((n.pointSize()+4)*zoom()/100);
    p.setFont(n);

    rc = rect();
    rc.adjust(p.fontMetrics().width("D")+getPinSize(),0,-1,-1);
    p.drawLine(rc.topLeft(),rc.bottomLeft());
    int dy = height()/6;
    int ddy = dy/2;
    p.drawLine(0,dy+ddy+getPinSize()/2,rc.left(),dy+ddy+getPinSize()/2);
    p.drawLine(0,3*dy+2*ddy+getPinSize()/2,rc.left(),3*dy+2*ddy+getPinSize()/2);

    rc.adjust(0,0,-p.fontMetrics().width("Q")-getPinSize(),-2*rc.height()/3);
    if (mode==TM_RSD)
        p.drawText(rc,Qt::AlignCenter,"T");
    else
        p.drawText(rc,Qt::AlignCenter,"TT");

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);
}

void QCPTrigger::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu cm(this);
    QAction* ac = cm.addAction(tr("RS/D trigger"),this,SLOT(applyDtrigger()));
    ac->setCheckable(true);
    ac->setChecked(mode==TM_RSD);
    ac = cm.addAction(tr("RS/T trigger"),this,SLOT(applyTtrigger()));
    ac->setCheckable(true);
    ac->setChecked(mode==TM_RST);
    cm.exec(event->globalPos());
}

void QCPTrigger::applyDtrigger()
{
    setMode(TM_RSD);
}

void QCPTrigger::applyTtrigger()
{
    setMode(TM_RST);
}
