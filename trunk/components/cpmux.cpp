#include "cpmux.h"
#include <QInputDialog>

QCPMux::QCPMux(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    dWidth = 4;
    dCount = 2;
    updateMuxPins();
}

QCPMux::~QCPMux()
{
    while (!fInputs.isEmpty()) {
        QCPInput* inp = fInputs.takeFirst();
        delete inp;
    }
    while (!fOutputs.isEmpty()) {
        QCPOutput* out = fOutputs.takeFirst();
        delete out;
    }
}

void QCPMux::updateMuxPins()
{
    deleteInputs();
    deleteOutputs();
    for (int i=0;i<ipow(2,dCount);i++) {
        for (int j=0;j<dWidth;j++) {
            if (i==0) {
                QCPOutput* out = new QCPOutput(this,this,tr("Q%1").arg(j)); fOutputs.append(out);
            }
            QCPInput* in = new QCPInput(this,this,tr("P%1%2").arg(QChar(0x41+i)).arg(j)); fInputs.append(in);
        }
    }
    for (int i=0;i<dCount;i++) {
        QCPInput* in = new QCPInput(this,this,tr("A%1").arg(i)); fInputs.append(in);
    }
    resize(minimumSizeHint());
    update();
}


QSize QCPMux::minimumSizeHint() const
{
    return QSize(110*zoom()/100,getDCompHeight(ipow(2,dCount)));
}

void QCPMux::readFromXML(QTextStream &errlog, const QDomElement &element)
{
    bool ok;
    dCount = element.attribute("channels","2").toInt(&ok);
    if ((!ok) || (dCount>8) || (dCount<1)) {
        errlog << tr("QCPMux: channels value incorrect") << endl;
        dCount = 2;
    }
    dWidth = element.attribute("dataWidth","2").toInt(&ok);
    if ((!ok) || (dWidth<1) || (dWidth>8)) {
        errlog << tr("QCPMux: dataWidth value incorrect") << endl;
        dWidth = 2;
    }

    updateMuxPins();
    QCPBase::readFromXML(errlog,element);
}

void QCPMux::storeToXML(QDomElement &element)
{
    element.setAttribute("channels",dCount);
    element.setAttribute("dataWidth",dWidth);
    QCPBase::storeToXML(element);
}

void QCPMux::realignPins(QPainter &)
{
    int dy = getDCompIncrement();
    int ddy = dy/2;
    int ddyc = 0;
    for (int i=0;i<fInputs.count();i++) {
        if (i>0) {
            if (fInputs[i-1]->pinName.at(0)!='A')
                if (fInputs[i]->pinName.left(2)!=fInputs[i-1]->pinName.left(2)) ddyc+=ddy;
        }
        fInputs[i]->relCoord = QPoint(getPinSize()/2,(i+1)*dy+ddyc);
        if (i<fOutputs.count())
            fOutputs[i]->relCoord = QPoint(width()-getPinSize()/2,(i+1)*dy+ddyc);
    }
}

void QCPMux::doLogicPrivate()
{
    qint32 di = 0;
    for (int i=0;i<dCount;i++)
        di = di | ((fInputs[i+ipow(2,dCount)*dWidth]->state & 0x1) << i);

    for (int i=0;i<dWidth;i++) {
        if ((di*dWidth+i)<fInputs.count())
            fOutputs[i]->state = fInputs[di*dWidth+i]->state;
        else
            fOutputs[i]->state = false;
    }
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
    rc.adjust(p.fontMetrics().width("PA0")+getPinSize()/2,0,-1,-1);
    p.drawLine(rc.topLeft(),rc.bottomLeft());

    int dy = getDCompIncrement();
    int ddy = dy/2;
    int ddyc = 0;
    for (int i=0;i<fInputs.count();i++) {
        if (i>0) {
            if (fInputs[i-1]->pinName.at(0)!='A')
                if (fInputs[i]->pinName.left(2)!=fInputs[i-1]->pinName.left(2)) {
                    ddyc+=ddy;
                    p.drawLine(0,i*dy+ddyc+getPinSize()/2,rc.left(),i*dy+ddyc+getPinSize()/2);
                }
        }
    }

    rc = rect();
    rc.adjust(width()-p.fontMetrics().width("A0")-getPinSize()/2,0,-1,-1);
    p.drawLine(rc.topLeft(),rc.bottomLeft());

    rc = rect();
    rc.adjust(0,0,-1,0);
    rc.adjust(p.fontMetrics().width("PA0")+getPinSize()/2,0,
              -p.fontMetrics().width("A0")-getPinSize()/2,0);
    rc.setHeight(getDCompIncrement()*3);
    n.setBold(false);
    p.setFont(n);
    p.drawText(rc,Qt::AlignCenter,"MX");

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);

    drawSelection(p);
}

void QCPMux::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu cm(this);
    cm.addAction(tr("Change address width..."),this,SLOT(changeAddrSize()));
    cm.addAction(tr("Change data width..."),this,SLOT(changeDataSize()));
    cm.exec(event->globalPos());
}

void QCPMux::changeAddrSize()
{
    bool ok;
    int tmp = QInputDialog::getInt(this,tr("Multiplexer params"),tr("Address bus width"),dCount,1,8,1,&ok);
    if (ok) {
        dCount = tmp;
        updateMuxPins();
    }
}

void QCPMux::changeDataSize()
{
    bool ok;
    int tmp = QInputDialog::getInt(this,tr("Multiplexer params"),tr("Data bus width"),dWidth,1,8,1,&ok);
    if (ok) {
        dWidth = tmp;
        updateMuxPins();
    }
}
