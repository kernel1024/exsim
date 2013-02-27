#include "cpmux.h"

QCPMux::QCPMux(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    dWidth = 2;
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
            QCPInput* in = new QCPInput(this,this,tr("%1%2").arg(QChar(0x49+i)).arg(j)); fInputs.append(in);
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
    return QSize(100*zoom()/100,getDCompHeight(ipow(2,dCount)));
}

void QCPMux::readFromStream(QDataStream &stream)
{
    stream >> dCount >> dWidth;
    updateMuxPins();
    QCPBase::readFromStream(stream);
}

void QCPMux::storeToStream(QDataStream &stream)
{
    stream << dCount << dWidth;
    QCPBase::storeToStream(stream);
}

void QCPMux::realignPins(QPainter &)
{
    int dy = getDCompIncrement();
    int ddy = dy/2;
    int ddyc = 0;
    for (int i=0;i<fInputs.count();i++) {
        if (i>0) {
            if (fInputs[i]->pinName.at(0)!=fInputs[i-1]->pinName.at(0)) ddyc+=ddy;
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
    rc.adjust(p.fontMetrics().width("A0")+getPinSize()/2,0,-1,-1);
    p.drawLine(rc.topLeft(),rc.bottomLeft());

    int dy = getDCompIncrement();
    int ddy = dy/2;
    int ddyc = 0;
    for (int i=0;i<fInputs.count();i++) {
        if (i>0) {
            if (fInputs[i]->pinName.at(0)!=fInputs[i-1]->pinName.at(0)) {
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
    rc.setHeight(getDCompIncrement()*3);
    n.setBold(false);
    p.setFont(n);
    p.drawText(rc,Qt::AlignCenter,"MX");

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);
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
