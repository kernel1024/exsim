#include "cpintextender.h"
#include <QInputDialog>

QCPIntExtender::QCPIntExtender(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    dWidth = 4;
    dCount = 2;
    updateMuxPins();
}

QCPIntExtender::~QCPIntExtender()
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

void QCPIntExtender::updateMuxPins()
{
    deleteInputs();
    deleteOutputs();
    for (int i=0;i<dCount;i++) {
        for (int j=0;j<dWidth;j++) {
            if (i==0) {
                QCPInput* in = new QCPInput(this,this,tr("%1").arg(j)); fInputs.append(in);
            }
            QCPOutput* out = new QCPOutput(this,this,tr("%1%2").arg(QChar(0x41+i)).arg(j));
            fOutputs.append(out);
        }
    }
    resize(minimumSizeHint());
    update();
}


QSize QCPIntExtender::minimumSizeHint() const
{
    return QSize(100*zoom()/100,getDCompHeight(dCount-1));
}

void QCPIntExtender::readFromXML(QTextStream &errlog, const QDomElement &element)
{
    bool ok;
    dCount = element.attribute("channels","2").toInt(&ok);
    if ((!ok) || (dCount>16) || (dCount<1)) {
        errlog << tr("QCPIntExtender: channels value incorrect") << endl;
        dCount = 2;
    }
    dWidth = element.attribute("dataWidth","2").toInt(&ok);
    if ((!ok) || (dWidth<1) || (dWidth>8)) {
        errlog << tr("QCPIntExtender: dataWidth value incorrect") << endl;
        dWidth = 2;
    }

    updateMuxPins();
    QCPBase::readFromXML(errlog,element);
}

void QCPIntExtender::storeToXML(QDomElement &element)
{
    element.setAttribute("channels",dCount);
    element.setAttribute("dataWidth",dWidth);
    QCPBase::storeToXML(element);
}

void QCPIntExtender::realignPins(QPainter &)
{
    int dy = getDCompIncrement();
    int ddy = dy/2;
    int ddyc = 0;
    for (int i=0;i<fOutputs.count();i++) {
        if (i>0) {
            if (fOutputs[i]->pinName.at(0)!=fOutputs[i-1]->pinName.at(0)) ddyc+=ddy;
        }
        fOutputs[i]->relCoord = QPoint(width()-getPinSize()/2,(i+1)*dy+ddyc);
        if (i<fInputs.count())
            fInputs[i]->relCoord = QPoint(getPinSize()/2,(i+1)*dy+ddyc);
    }
}

void QCPIntExtender::doLogicPrivate()
{
    for (int i=0;i<dCount;i++)
        for (int j=0;j<dWidth;j++) {
            if ((i*dWidth+j)<fOutputs.count())
                fOutputs[i*dWidth+j]->state = fInputs[j]->state;
        }
}

void QCPIntExtender::paintEvent(QPaintEvent *)
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
    rc = rect();
    rc.adjust(width()-p.fontMetrics().width("A0")-getPinSize()/2,0,-1,-1);
    p.drawLine(rc.topLeft(),rc.bottomLeft());
    for (int i=0;i<fOutputs.count();i++) {
        if (i>0) {
            if (fOutputs[i]->pinName.at(0)!=fOutputs[i-1]->pinName.at(0)) {
                ddyc+=ddy;
                p.drawLine(rc.left(),i*dy+ddyc+getPinSize()/2,rc.right(),i*dy+ddyc+getPinSize()/2);
            }
        }
    }

    rc = rect();
    rc.adjust(0,0,-1,0);
    rc.adjust(p.fontMetrics().width("A0")+getPinSize()/2,0,
              -p.fontMetrics().width("A0")-getPinSize()/2,0);
    rc.setHeight(getDCompIncrement()*3);
    n.setBold(false);
    p.setFont(n);
    p.drawText(rc,Qt::AlignCenter,"EX");

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);

    drawSelection(p);
}

void QCPIntExtender::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu cm(this);
    cm.addAction(tr("Change channels count..."),this,SLOT(changeChannelsCount()));
    cm.addAction(tr("Change data width..."),this,SLOT(changeDataSize()));
    cm.exec(event->globalPos());
}

void QCPIntExtender::changeChannelsCount()
{
    bool ok;
    int tmp = QInputDialog::getInt(this,tr("Extender params"),tr("Address bus width"),dCount,1,16,1,&ok);
    if (ok) {
        dCount = tmp;
        updateMuxPins();
    }
}

void QCPIntExtender::changeDataSize()
{
    bool ok;
    int tmp = QInputDialog::getInt(this,tr("Extender params"),tr("Data bus width"),dWidth,1,8,1,&ok);
    if (ok) {
        dWidth = tmp;
        updateMuxPins();
    }
}
