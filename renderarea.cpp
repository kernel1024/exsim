#include "renderarea.h"
#include "mainwindow.h"

#include "components/cpbutton.h"
#include "components/cpled.h"
#include "components/cplogic.h"
#include "components/cpextender.h"
#include "components/cptrigger.h"
#include "components/cpgen.h"
#include "components/cpbeeper.h"
#include "components/cpregister.h"
#include "components/cpdecoder.h"
#include "components/cpencoder.h"
#include "components/cpcounter.h"
#include "components/cpdigit.h"
#include "components/cpmux.h"
#include "components/cpsum.h"
#include "components/cpgnd.h"
#include "components/cpvcc.h"
#include "components/cpcomparator.h"
#include "components/cprndgen.h"
#include "components/cpram.h"
#include "components/cpsynth.h"
#include "components/cplabel.h"
#include "components/cpintextender.h"

QRenderArea::QRenderArea(QWidget *parent, QScrollArea *aScroller, int aLcdFontIdx)
    : QFrame(parent)
{
    scroller=aScroller;
    pendingLogicLinks.clear();

    resReading=false;
    zoom = 100;
    nodeLocks.clear();

    recycle=new QLabel(this);
    recycle->setGeometry(QRect(10, 10, 48, 48));
    recycle->setPixmap(QPixmap(":/images/trashcan_empty.png"));
    recycle->setText("");
    recycle->setObjectName("trashcan");

    cbType=0;
    cbPinNum=-1;
    cbConnCount=0;
    cbInput=0;
    cbOutput=0;
    cbBuilding=false;
    cbCurrent=QPoint(0,0);
    pinColorOff=Qt::blue;
    pinColorOn=Qt::red;

    selectionPoint=QPoint();
    selectionBox = new QRubberBand(QRubberBand::Rectangle,this);
    selectionBox->hide();
    selectionBox->setGeometry(QRect());
    selection.clear();

    setMouseTracking(true);

    lcdFont=QApplication::font();
    if (aLcdFontIdx>=0) {
        if (!QFontDatabase::applicationFontFamilies(aLcdFontIdx).isEmpty()) {
            QString font = QFontDatabase::applicationFontFamilies(aLcdFontIdx).first();
            if (!font.isEmpty())
                lcdFont=QFont(font);
        }
    }

}

QSize QRenderArea::minimumSizeHint() const
{
    int x=3*scroller->width()/2;
    int y=3*scroller->height()/2;
    QRect r(0,0,0,0);
    for (int i=0;i<children().count();i++)
        if (QWidget* w=qobject_cast<QWidget*>(children().at(i)))
            r=r.united(w->geometry());
    r.adjust(-200,-200,200,200);
    QSize cmSize=QSize(r.width(),r.height());
    return QSize(x,y).expandedTo(cmSize);
}

QSize QRenderArea::sizeHint() const
{
    return minimumSizeHint();
}

void QRenderArea::initConnBuilder(const int aType, int aPinNum, QCPInput* aInput, QCPOutput* aOutput)
{
    if (cbBuilding)
    {
        cbBuilding=false;
        return;
    }
    cbType=aType;
    cbPinNum=aPinNum;
    cbInput=0;
    cbOutput=0;
    // init connection form input
    if (cbType==QPT_INPUT)
    {
        cbInput=aInput;
        if (cbInput!=0)
        {
            // disconnect old connections from this input
            if ((cbInput->fromPin!=-1) && (cbInput->fromCmp!=0))
            {
                cbInput->fromCmp->fOutputs[cbInput->fromPin]->toCmp=0;
                cbInput->fromCmp->fOutputs[cbInput->fromPin]->toPin=-1;
            }
            cbInput->fromCmp=0;
            cbInput->fromPin=-1;
            cbCurrent=cbInput->ownerCmp->pos()+cbInput->relCoord;
        }
    } else {
        // init connection from output
        cbOutput=aOutput;
        if (cbOutput!=0)
        {
            // disconnect old connections from this output
            if ((cbOutput->toPin!=-1) && (cbOutput->toCmp!=0))
            {
                cbOutput->toCmp->fInputs[cbOutput->toPin]->fromCmp=0;
                cbOutput->toCmp->fInputs[cbOutput->toPin]->fromPin=-1;
            }
            cbOutput->toCmp=0;
            cbOutput->toPin=-1;
            cbCurrent=cbOutput->ownerCmp->pos()+cbOutput->relCoord;
        }
    }
    cbBuilding=true;
}

void QRenderArea::repaintConn()
{
    repaint();
}

void QRenderArea::paintEvent ( QPaintEvent * )
{
    QPainter p(this);
    QPoint c1,c2;

    QFont nf=QApplication::font();
    nf.setPointSize(8*zoom/100);
    QFont of=p.font();
    QPen op=p.pen();
    p.setPen(QPen(Qt::red));
    p.setFont(nf);

    paintConnections(&p);

    p.setPen(QPen(Qt::red));
    if (cbBuilding)
    {
        p.setPen(QPen(Qt::darkCyan));
        if (cbType==QPT_INPUT)
            c1=cbInput->ownerCmp->pos()+cbInput->relCoord;
        else
            c1=cbOutput->ownerCmp->pos()+cbOutput->relCoord;
        c2=cbCurrent;
        p.drawLine(c1,c2);
    }

    p.setPen(op);
    p.setFont(of);
}

void QRenderArea::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton)>0 && !selectionPoint.isNull()) {
        selectionBox->setGeometry(QRect(event->pos(),selectionPoint).normalized());
        event->accept();
    }
}

void QRenderArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button()==Qt::LeftButton) {
        selectionPoint = event->pos();
        selectionBox->setGeometry(selectionPoint.x(),selectionPoint.y(),0,0);
        selectionBox->show();
        event->accept();
    }
}

void QRenderArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button()==Qt::LeftButton) {
        selection.clear();
        if (!selectionPoint.isNull()) {
            QRect cp = QRect(event->pos(),selectionPoint).normalized();
            for (int i=0;i<children().count();i++)
            {
                QCPBase* base=qobject_cast<QCPBase*>(children().at(i));
                if (base==NULL) continue;
                if (cp.contains(base->geometry(),true))
                    selection << base;
            }
        }
        event->accept();
        update();
    }
    selectionPoint=QPoint();
    selectionBox->setGeometry(QRect());
    selectionBox->hide();
}

void QRenderArea::paintConnections(QPainter* p)
{
    const int pinSpace = 20*zoom/100;

    QCPOutput* aout;
    QCPInput* ainp;
    QCPBase *base;

    QList<QCPBase *> cmps;
    QList<QRect> cmpsRect;

    QPen wirePen = QPen(Qt::red);
    QPen busPen = QPen(Qt::blue);
    busPen.setWidth(3);
    QPen textPen = QPen(Qt::black);
    QFontMetrics fm(p->font());

    cmps.clear();
    cmpsRect.clear();
    cbConnCount=0;

    QRect framingRect = QRect();

    for (int i=0;i<children().count();i++) {
        base=qobject_cast<QCPBase*>(children().at(i));
        if (base!=NULL) {
            cmps << base;
            cmpsRect << base->geometry();
            framingRect = framingRect.united(base->geometry());
        }
    }

    for (int i=0;i<cmps.count();i++) {
        base = cmps.at(i);
        for (int j=0;j<base->fOutputs.count();j++) {
            aout=base->fOutputs[j];
            if (aout->toPin==-1) continue;
            if (aout->toCmp==0) continue;
            ainp=aout->toCmp->fInputs[aout->toPin];

            QPoint c1 = base->pos()+aout->relCoord;
            QPoint c2 = aout->toCmp->pos()+ainp->relCoord;

            QRect linkRect = QRect(c1,c2).normalized();
            linkRect.adjust(base->getPinSize()/2,0,-base->getPinSize()/2-2,0);
            bool selfIntersect = (linkRect.intersects(base->geometry()) ||
                    linkRect.intersects(aout->toCmp->geometry()));

            linkRect = QRect(c1,c2).normalized();
            if (selfIntersect)
                linkRect.adjust(-base->getPinSize()/2-2,0,base->getPinSize()/2,0);
            else
                linkRect.adjust(base->getPinSize()/2,0,-base->getPinSize()/2-1,0);

            // Check intersection between line framing rect and any components
            bool intsno = true;
            QList<int> intsidx;
            intsidx.clear();
            for (int k=0;k<cmpsRect.count();k++) {
                if (linkRect.intersects(cmpsRect.at(k))) {
                    intsno = false;
                    intsidx << k;
                }
            }

            // If no intersections occurs - simply draw 3-part line
            if (intsno) {
                QPoint c3=QPoint((c1.x()+c2.x())/2,c1.y());
                QPoint c4=QPoint(c3.x(),c2.y());

                p->setPen(wirePen);
                p->drawLine(c1,c3);
                p->drawLine(c3,c4);
                p->drawLine(c4,c2);
            } else {
                int si = -1;
                if (selfIntersect) si = 1;

                QPoint p1, p2, m1, m2;
                double dy = c1.y()-c2.y();
                double dx = c1.x()-c2.x();
                if ((dy/dx)<0) {
                    p1 = QPoint(linkRect.right()+si*pinSpace,linkRect.top());
                    p2 = QPoint(linkRect.left()-si*pinSpace,linkRect.bottom());
                    c1 = linkRect.topRight();
                    c2 = linkRect.bottomLeft();
                    m1 = QPoint(linkRect.right()+si*pinSpace,framingRect.bottom()+pinSpace);
                    m2 = QPoint(linkRect.left()-si*pinSpace,framingRect.bottom()+pinSpace);
                } else {
                    p1 = QPoint(linkRect.left()-si*pinSpace,linkRect.top());
                    p2 = QPoint(linkRect.right()+si*pinSpace,linkRect.bottom());
                    c1 = linkRect.topLeft();
                    c2 = linkRect.bottomRight();
                    m1 = QPoint(linkRect.left()-si*pinSpace,framingRect.bottom()+pinSpace);
                    m2 = QPoint(linkRect.right()+si*pinSpace,framingRect.bottom()+pinSpace);
                }
                QString busID = tr("%1").arg(cbConnCount+1);
                QRect rt1 = QRect(c1,p1).normalized();
                rt1.setHeight(fm.height());
                rt1.adjust(0,-fm.height(),0,-fm.height());
                QRect rt2 = QRect(c2,p2).normalized();
                rt2.setHeight(fm.height());
                rt2.adjust(0,-fm.height(),0,-fm.height());
                p->setPen(wirePen);
                p->drawLine(c1,p1);
                p->drawLine(p2,c2);
                p->setPen(busPen);
                p->drawLine(p1,m1);
                p->drawLine(m1,m2);
                p->drawLine(m2,p2);
                p->setPen(textPen);
                p->drawText(rt1,Qt::AlignCenter,busID);
                p->drawText(rt2,Qt::AlignCenter,busID);
            }
            cbConnCount++;
        }
    }
    cmps.clear();
    cmpsRect.clear();
}

void QRenderArea::refreshConnBuilder(const QPoint & atPos)
{
    if (!cbBuilding) return;
    if (cbType==QPT_INPUT)
        cbCurrent=QPoint(cbInput->ownerCmp->pos()+atPos);
    else
        cbCurrent=QPoint(cbOutput->ownerCmp->pos()+atPos);
    repaintConn();
}

void QRenderArea::doneConnBuilder(const bool aNone, int aType,
                                  const int aPinNum, QCPInput* aInput, QCPOutput* aOutput)
{
    // if we making trace from input to this output...
    if ((cbType==QPT_INPUT) && (cbInput!=0))
    {
        // and we have new output now...
        if (aOutput!=0)
        {
            // then we remove old connection to this output to connect our new trace to it
            if ((aOutput->toPin!=-1) && (aOutput->toCmp!=0))
            {
                aOutput->toCmp->fInputs[aOutput->toPin]->fromCmp=0;
                aOutput->toCmp->fInputs[aOutput->toPin]->fromPin=-1;
            }
            aOutput->toCmp=0;
            aOutput->toPin=-1;
            aOutput->ownerCmp->regroupOutputs();
        }
        // if our input (from that we making connection) is connected - then disconnect it now
        if ((cbInput->fromPin!=-1) && (cbInput->fromCmp!=0))
        {
            cbInput->fromCmp->fOutputs[cbInput->fromPin]->toCmp=0;
            cbInput->fromCmp->fOutputs[cbInput->fromPin]->toPin=-1;
            cbInput->fromCmp->regroupOutputs();
        }
        cbInput->fromCmp=0;
        cbInput->fromPin=-1;
        cbInput->applyState(false);
    }
    // if we making trace from output to this input...
    else if (cbOutput!=0)
    {
        // and we have new input now...
        if (aInput!=0)
        {
            // then we remove old connection to this input
            if ((aInput->fromPin!=-1) && (aInput->fromCmp!=0))
            {
                aInput->fromCmp->fOutputs[aInput->fromPin]->toCmp=0;
                aInput->fromCmp->fOutputs[aInput->fromPin]->toPin=-1;
                aInput->fromCmp->regroupOutputs();
            }
            aInput->fromCmp=0;
            aInput->fromPin=-1;
            aInput->applyState(false);
        }
        // if our output (from that we making connection) is connected - then disconnect it now
        if ((cbOutput->toPin!=-1) && (cbOutput->toCmp!=0))
        {
            cbOutput->toCmp->fInputs[cbOutput->toPin]->fromCmp=0;
            cbOutput->toCmp->fInputs[cbOutput->toPin]->fromPin=-1;
        }
        cbOutput->toCmp=0;
        cbOutput->toPin=-1;
        cbOutput->ownerCmp->regroupOutputs();
    }
    // if this is simple deletion or incorrect route (in-in, out-out), then delete it
    if ((aNone) || (aType==cbType))
    {
        cbBuilding=false;
        repaintConn();
        return;
    }
    // if this output can't possible connect to specified input, then delete it
    QCPBase *aTo, *aFrom;
    if (cbType==QPT_INPUT)
    {
        aTo=cbInput->ownerCmp;
        aFrom=aOutput->ownerCmp;
    } else {
        aTo=aInput->ownerCmp;
        aFrom=cbOutput->ownerCmp;
    }
    if ((!aFrom->canConnectOut(aTo)) || (!aTo->canConnectIn(aFrom)))
    {
        cbBuilding=false;
        repaintConn();
        return;
    }
    if (cbType==QPT_INPUT)
    {
        cbInput->fromCmp=aOutput->ownerCmp;
        cbInput->fromPin=aPinNum;
        aOutput->toCmp=cbInput->ownerCmp;
        aOutput->toPin=cbPinNum;
        aOutput->ownerCmp->regroupOutputs();
        cbInput->applyState(aOutput->state);
    } else {
        cbOutput->toCmp=aInput->ownerCmp;
        cbOutput->toPin=aPinNum;
        aInput->fromCmp=cbOutput->ownerCmp;
        aInput->fromPin=cbPinNum;
        cbOutput->ownerCmp->regroupOutputs();
        aInput->applyState(cbOutput->state);
    }
    cbBuilding=false;
    repaintConn();
}

void QRenderArea::postLoadBinding(QTextStream &errlog)
{
    pendingLogicLinks.clear();
    for (int i=0;i<children().count();i++)
    {
        QCPBase* base=qobject_cast<QCPBase*>(children().at(i));
        if (base!=0) base->postLoadBind(errlog);
    }
}

int QRenderArea::cpComponentCount()
{
    int c=0;
    for (int i=0;i<children().count();i++)
        if (qobject_cast<QCPBase*>(children().at(i)) != NULL) c++;
    return c;
}

void QRenderArea::setZoom(int zoomFactor)
{
    zoom = zoomFactor;

    recycle->setGeometry(QRect(10*zoom/100, 10*zoom/100, 48*zoom/100, 48*zoom/100));
    QPixmap p = QPixmap(":/images/trashcan_empty.png");
    if (zoom!=100)
        p = p.scaled(48*zoom/100,48*zoom/100,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    recycle->setPixmap(p);

    for (int i=0;i<children().count();i++) {
        QCPBase* cb = qobject_cast<QCPBase*>(children().at(i));
        if (cb != NULL) {
            cb->zoomChanged();
        }
    }
    repaintConn();
}

void QRenderArea::readSchematic(QTextStream &errlog, const QDomElement &element,
                                const QPoint indent, const bool addBlock)
{
    selection.clear();

    if (element.isNull())
        errlog << tr("readSchematic: NULL element passed") << endl;

    if (!addBlock) {
        pinColorOff = QColor(element.attribute("pinColorOff","blue"));
        pinColorOn = QColor(element.attribute("pinColorOn","red"));
        if (!pinColorOff.isValid()) {
            errlog << tr("readSchematic: pinColorOff has incorrect name") << endl;
            pinColorOff = QColor(Qt::blue);
        }
        if (!pinColorOn.isValid()) {
            errlog << tr("readSchematic: pinColorOn has incorrect name") << endl;
            pinColorOn = QColor(Qt::red);
        }
    }

    for (int i=0;i<element.childNodes().count();i++)
    {
        if (!element.childNodes().at(i).isElement()) continue;
        QDomElement xc = element.childNodes().at(i).toElement();
        QString clName = xc.attribute("className","---");
        QCPBase* b=createCpInstance(clName);
        if (b==0)
        {
            errlog << tr("readSchematic: class '%1' not found.").arg(clName) << endl;
        } else {
            QPoint pos = QPoint(200,200);
            QStringList spos = xc.attribute("position","200,200").split(',',QString::SkipEmptyParts);
            if (spos.count()!=2)
                errlog << tr("readSchematic: wrong element '%1' position").arg(clName) << endl;
            else {
                bool ok1, ok2;
                int x = spos.first().toInt(&ok1);
                int y = spos.last().toInt(&ok2);
                if (!ok1 || !ok2) {
                    errlog << tr("readSchematic: wrong element '%1' position").arg(clName) << endl;
                    x = 200; y = 200;
                }
                pos = QPoint(x,y) + indent;
            }
            QString instName = xc.attribute("instanceName",tr("QCPBase%1").arg(i+1));
            b->move(pos);
            b->setObjectName(instName);
            b->readFromXML(errlog, xc);
            b->show();
            if (addBlock)
                selection << b;
        }
    }
    resReading=false;
    if (addBlock) {
        for(int i=0;i<selection.count();i++)
            selection[i]->postLoadBind(errlog);
    } else
        postLoadBinding(errlog);
    repaintConn();
}

void QRenderArea::storeSchematic(QDomElement &element)
{
    element.setAttribute("pinColorOff",pinColorOff.name());
    element.setAttribute("pinColorOn",pinColorOn.name());

    for (int i=0;i<children().count();i++)
    {
        QCPBase* base=qobject_cast<QCPBase*>(children().at(i));
        if (!base) continue;

        QDomElement xc = element.ownerDocument().createElement(tr("element%1").arg(i));
        xc.setAttribute("className",base->metaObject()->className());
        xc.setAttribute("position",tr("%1,%2").arg(base->pos().x()).arg(base->pos().y()));
        xc.setAttribute("instanceName",base->objectName());
        base->storeToXML(xc);
        element.appendChild(xc);
    }
}

void QRenderArea::deleteComponents()
{
    selection.clear();
    for (int i=0;i<children().count();i++)
    {
        QCPBase* base=qobject_cast<QCPBase*>(children().at(i));
        if (base!=0) base->deleteLater();
    }
    repaintConn();
}

QCPBase* QRenderArea::createCpInstance(const QString &className)
{
    if (className=="QCPButton")      return new QCPButton(this,this);
    else if (className=="QCPLed")      return new QCPLed(this,this);
    else if (className=="QCPLogic")      return new QCPLogic(this,this);
    else if (className=="QCPExtender")      return new QCPExtender(this,this);
    else if (className=="QCPTrigger")      return new QCPTrigger(this,this);
    else if (className=="QCPGen")      return new QCPGen(this,this);
    else if (className=="QCPBeeper")      return new QCPBeeper(this,this);
    else if (className=="QCPRegister")      return new QCPRegister(this,this);
    else if (className=="QCPDecoder")      return new QCPDecoder(this,this);
    else if (className=="QCPEncoder")      return new QCPEncoder(this,this);
    else if (className=="QCPCounter")      return new QCPCounter(this,this);
    else if (className=="QCPDigit")      return new QCPDigit(this,this);
    else if (className=="QCPMux")      return new QCPMux(this,this);
    else if (className=="QCPSum")      return new QCPSum(this,this);
    else if (className=="QCPGnd")      return new QCPGnd(this,this);
    else if (className=="QCPVcc")      return new QCPVcc(this,this);
    else if (className=="QCPComparator")      return new QCPComparator(this,this);
    else if (className=="QCPRndGen")      return new QCPRndGen(this,this);
    else if (className=="QCPRAM")      return new QCPRAM(this,this);
    else if (className=="QCPSynth")      return new QCPSynth(this,this);
    else if (className=="QCPLabel")      return new QCPLabel(this,this);
    else if (className=="QCPIntExtender")      return new QCPIntExtender(this,this);
    else return NULL;
}
