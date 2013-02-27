#include "renderarea.h"
#include "cpbase.h"

int ipow(int base, int pow)
{
    qlonglong t = base;
    if (pow == 0) t = 1;
    for (int i=1;i<pow;i++)
        t = t*base;
    return t;
}

QCPBase::QCPBase(QWidget *parent, QRenderArea *aOwner)
    : QWidget(parent)
{
    pinColorOff=Qt::blue;
    pinColorOn=Qt::red;
    isDragging=false;
    cpOwner=aOwner;
    oldZoom=100;
    savedState=QString();
    fInputs.clear();
    fOutputs.clear();
    setContextMenuPolicy(Qt::DefaultContextMenu);
}

QSize QCPBase::minimumSizeHint() const
{
    return QSize(50,50);
}

QSize QCPBase::sizeHint() const
{
    return minimumSizeHint();
}

bool QCPBase::canConnectOut(QCPBase *)
{
    return true;
}

bool QCPBase::canConnectIn(QCPBase *)
{
    return true;
}

void QCPBase::mouseInPin(const QPoint & mx, int &aPinNum, int &aPinType, QCPBase* & aFilter)
// Note: x and y must be in global screen coordinate system.
{
    QRect r;
    QPoint p;
    aPinNum=-1;
    aPinType=QPT_INPUT;
    aFilter=0;
    for (int i=0; i<cpOwner->children().count(); i++)
    {
        QObject* oc=cpOwner->children().at(i);
        QCPBase *c=qobject_cast<QCPBase*>(oc);
        if (c==0) continue;
        for (int j=0; j<c->fInputs.count(); j++)
        {
            QCPInput* a=c->fInputs[j];
            p=QPoint(cpOwner->mapToGlobal(QPoint(a->ownerCmp->x()+a->relCoord.x()+getPinSize()/2,
                                                 a->ownerCmp->y()+a->relCoord.y()+getPinSize()/2)));
            r=QRect(p.x()-getPinSize(),p.y()-getPinSize(),
                    2*getPinSize(),2*getPinSize());
            if (r.contains(mx.x(),mx.y()))
            {
                aPinNum=j;
                aPinType=QPT_INPUT;
                aFilter=c;
                return;
            }
        }

        for (int j=0; j<c->fOutputs.count(); j++)
        {
            QCPOutput* a=c->fOutputs[j];
            p=QPoint(cpOwner->mapToGlobal(QPoint(a->ownerCmp->x()+a->relCoord.x()+getPinSize()/2,
                                                 a->ownerCmp->y()+a->relCoord.y()+getPinSize()/2)));
            r=QRect(p.x()-getPinSize(),p.y()-getPinSize(),
                    2*getPinSize(),2*getPinSize());
            if (r.contains(mx.x(),mx.y()))
            {
                aPinNum=j;
                aPinType=QPT_OUTPUT;
                aFilter=c;
                return;
            }
        }
    }
}

void QCPBase::redrawPins(QPainter & painter)
{
    if (cpOwner->resReading) return;
    realignPins(painter);

    QPen op=painter.pen();
    QBrush ob=painter.brush();
    QFont of=painter.font();
    QFont n=QApplication::font();
    QColor pc = Qt::black;
    n.setBold(true);
    n.setPointSize(n.pointSize()*zoom()/100);
    painter.setFont(n);
    for (int i=0;i<fInputs.count();i++)
    {
        QCPInput* a=fInputs[i];
        if (a->state)
            pc = pinColorOn;
        else
            pc = pinColorOff;
        painter.setPen(QPen(pc));
        QBrush brshPin = QBrush(pc,Qt::SolidPattern);
        painter.setBrush(brshPin);
        QRect pinRect = QRect(   a->relCoord.x()-getPinSize()/2,
                                 a->relCoord.y()-getPinSize()/2,
                                 getPinSize(),
                                 getPinSize());
        if (!a->inversed)
            painter.fillRect(pinRect,brshPin);
        else
            painter.drawEllipse(pinRect);
        if (a->pinName.isEmpty()) continue;
        painter.setPen(QPen(Qt::black));
        painter.drawText(QPoint(  a->relCoord.x()+getPinSize()/2+1,
                                  a->relCoord.y()+painter.fontMetrics().height()/4),
                         a->pinName);
        if (a->inversed)
            painter.drawLine(QPoint(  a->relCoord.x()+getPinSize()/2+1,
                                      a->relCoord.y()-2*painter.fontMetrics().height()/3),
                             QPoint(  a->relCoord.x()+getPinSize()/2+1+painter.fontMetrics().width(a->pinName),
                                      a->relCoord.y()-2*painter.fontMetrics().height()/3));

    }
    for (int i=0;i<fOutputs.count();i++)
    {
        QCPOutput* a=fOutputs[i];
        if (a->state)
            pc = pinColorOn;
        else
            pc = pinColorOff;
        painter.setPen(QPen(pc));
        QBrush brshPin = QBrush(pc,Qt::SolidPattern);
        painter.setBrush(brshPin);
        QRect pinRect = QRect(   a->relCoord.x()-getPinSize()/2,
                                  a->relCoord.y()-getPinSize()/2,
                                  getPinSize(),
                                  getPinSize());
        if (!a->inversed)
            painter.fillRect(pinRect,brshPin);
        else
            painter.drawEllipse(pinRect);
        if (a->pinName.isEmpty()) continue;
        painter.setPen(QPen(Qt::black));
        painter.drawText(QPoint(  a->relCoord.x()-getPinSize()/2-1 - painter.fontMetrics().width(a->pinName),
                                  a->relCoord.y()+painter.fontMetrics().height()/4), a->pinName);
        if (a->inversed)
            painter.drawLine(QPoint(  a->relCoord.x()-getPinSize()/2-1 -
                                      painter.fontMetrics().width(a->pinName),
                                      a->relCoord.y()-2*painter.fontMetrics().height()/3),
                             QPoint(  a->relCoord.x()-getPinSize()/2-1,
                                      a->relCoord.y()-2*painter.fontMetrics().height()/3));
    }
    painter.setFont(of);
    painter.setBrush(ob);
    painter.setPen(op);
}

void QCPBase::zoomChanged()
{
    QPoint newpos = pos()*(100.0/oldZoom);
    oldZoom=zoom();
    move(newpos*(oldZoom/100.0));
    resize(minimumSizeHint());
}

int QCPBase::getPinSize() const
{
    return 8*zoom()/100;
}

void QCPBase::postLoadBind(QTextStream &errlog)
{
    for (int i=0;i<fInputs.count();i++)
        fInputs[i]->postLoadBind(errlog);
    for (int i=0;i<fOutputs.count();i++)
        fOutputs[i]->postLoadBind(errlog);
    regroupOutputs();
}

void QCPBase::regroupOutputs()
{
    for (int i=0;i<fOutputs.count();i++)
        fOutputs[i]->groupId=QUuid();

    if (!fOutputs.isEmpty()) {
        if (fOutputs[0]->toCmp!=NULL)
            fOutputs[0]->groupId=QUuid::createUuid();
        for (int i=0;i<fOutputs.count();i++) {
            if (fOutputs[i]->toCmp==NULL) continue;

            for (int j=0;j<fOutputs.count();j++) {
                if (j==i) continue;
                else {
                    if ((fOutputs[i]->toCmp==fOutputs[j]->toCmp) &&
                            (!fOutputs[j]->groupId.isNull())) {
                        fOutputs[i]->groupId=fOutputs[j]->groupId;
                        break;
                    }
                }
            }
            if (fOutputs[i]->groupId.isNull())
                fOutputs[i]->groupId=QUuid::createUuid();
        }
    }
}

void QCPBase::checkRecycle(bool forceDelete)
{
    if ((!forceDelete) && (!(frameGeometry().intersects(cpOwner->recycle->frameGeometry())))) return;
    for (int i=0;i<fInputs.count();i++)
    {
        if (fInputs[i]->fromPin!=-1)
            if (fInputs[i]->fromCmp!=0)
            {
                fInputs[i]->fromCmp->fOutputs[fInputs[i]->fromPin]->toCmp=0;
                fInputs[i]->fromCmp->fOutputs[fInputs[i]->fromPin]->toPin=-1;
            }
        fInputs[i]->fromCmp=0;
        fInputs[i]->fromPin=-1;
    }
    for (int i=0;i<fOutputs.count();i++)
    {
        if (fOutputs[i]->toPin!=-1)
            if (fOutputs[i]->toCmp!=0)
            {
                fOutputs[i]->toCmp->fInputs[fOutputs[i]->toPin]->fromCmp=0;
                fOutputs[i]->toCmp->fInputs[fOutputs[i]->toPin]->fromPin=-1;
            }
        fOutputs[i]->toCmp=0;
        fOutputs[i]->toPin=-1;
    }
    cpOwner->repaintConn();
    deleteLater();
}

void QCPBase::deleteInputs()
{
    for (int i=0;i<fInputs.count();i++) {
        QCPInput* cbInput = fInputs.at(i);
        if ((cbInput->fromPin!=-1) && (cbInput->fromCmp!=0))
        {
            cbInput->fromCmp->fOutputs[cbInput->fromPin]->toCmp=0;
            cbInput->fromCmp->fOutputs[cbInput->fromPin]->toPin=-1;
        }
        cbInput->fromCmp=0;
        cbInput->fromPin=-1;
        cbInput->deleteLater();
    }
    fInputs.clear();
}

void QCPBase::deleteOutputs()
{
    for (int i=0;i<fOutputs.count();i++) {
        QCPOutput* cbOutput = fOutputs.at(i);
        if ((cbOutput->toPin!=-1) && (cbOutput->toCmp!=0))
        {
            cbOutput->toCmp->fInputs[cbOutput->toPin]->fromCmp=0;
            cbOutput->toCmp->fInputs[cbOutput->toPin]->fromPin=-1;
        }
        cbOutput->toCmp=0;
        cbOutput->toPin=-1;
        cbOutput->deleteLater();
    }
    fOutputs.clear();
}

int QCPBase::zoom() const
{
    return cpOwner->zoom;
}

bool QCPBase::isStateChanged()
{
    if (fInputs.isEmpty()) // initiator component (button, generator)
        return true;
    return (calcState()!=savedState);
}

QString QCPBase::calcState()
{
    QString a = QString();
    for(int i=0;i<fInputs.count();i++) {
        if (fInputs.at(i)->state)
            a += "1";
        else
            a += "0";
    }
    return a;
}

int QCPBase::getDCompHeight(const int divCount) const
{
    int dy = getDCompIncrement();
    int vsz = (dy * (qMax(fInputs.count(),fOutputs.count())+1)) + ((dy * divCount)/2);
    if (vsz<5) vsz = 5;
    return vsz;
}

int QCPBase::getDCompIncrement() const
{
    return ((QApplication::fontMetrics().height()+4)*zoom()/100);
}

void QCPBase::mouseMoveEvent(QMouseEvent * event)
{
    if (isDragging)
    {
        move(QPoint(x()+event->x()-relCorner.x(),y()+event->y()-relCorner.y()));
        cpOwner->repaintConn();
        cpOwner->resize(cpOwner->minimumSizeHint());
        update();
        emit componentChanged(this);
    } else
        cpOwner->refreshConnBuilder(event->pos());

}

void QCPBase::paintEvent ( QPaintEvent * )
{
    QPainter p(this);
    p.setPen(QPen(Qt::black));
    p.setBrush(QBrush(Qt::white,Qt::SolidPattern));

    p.drawRect(rect());
}

void QCPBase::mousePressEvent(QMouseEvent * event)
{
    raise();
//    if (event->button()==Qt::RightButton)
//        QWidget::mousePressEvent(event);
    QPoint mx=mapToGlobal(event->pos());
    QCPBase* dFlt;
    int pNum,pType;
    mouseInPin(mx,pNum,pType,dFlt);
    emit componentChanged(this);
    if (pNum==-1)
    {
        relCorner=event->pos();
        isDragging=true;
        return;
    }
    isDragging=false;
    if (pType==QPT_INPUT)
        cpOwner->initConnBuilder(QPT_INPUT,pNum,dFlt->fInputs[pNum],0);
    else
        cpOwner->initConnBuilder(QPT_OUTPUT,pNum,0,dFlt->fOutputs[pNum]);
}

void QCPBase::mouseReleaseEvent(QMouseEvent * event)
{
    QPoint mx=mapToGlobal(event->pos());
    QCPBase* dFlt;
    int pNum,pType;
    mouseInPin(mx,pNum,pType,dFlt);
    if (pNum==-1)
    {
        bool f=isDragging;
        isDragging=false;
        if (!f)
            cpOwner->doneConnBuilder(true,QPT_INPUT,-1,0,0);
        else
            checkRecycle();
        isDragging=f;
        return;
    }
    if (pType==QPT_INPUT)
        cpOwner->doneConnBuilder(false,QPT_INPUT,pNum,dFlt->fInputs[pNum],0);
    else
        cpOwner->doneConnBuilder(false,QPT_OUTPUT,pNum,0,dFlt->fOutputs[pNum]);
    emit componentChanged(this);
}

void QCPBase::readFromStream(QTextStream &errlog, const QDomElement &element)
{
    pinColorOff = QColor(element.attribute("pinColorOff","blue"));
    pinColorOn = QColor(element.attribute("pinColorOn","red"));
    if (!pinColorOff.isValid()) {
        errlog << tr("QCPBase: pinColorOff has incorrect name");
        pinColorOff = QColor(Qt::blue);
    }
    if (!pinColorOn.isValid()) {
        errlog << tr("QCPBase: pinColorOn has incorrect name");
        pinColorOn = QColor(Qt::red);
    }

    QDomElement inp = element.firstChildElement("inputs");
    QDomElement out = element.firstChildElement("outputs");

    for (int i=0;i<fInputs.count();i++)
        fInputs[i]->readFromStream(errlog,inp.firstChildElement(tr("inp%1").arg(i)));
    for (int i=0;i<fOutputs.count();i++)
        fOutputs[i]->readFromStream(errlog,out.firstChildElement(tr("out%1").arg(i)));
}

void QCPBase::storeToStream(QDomElement &element)
{
    element.setAttribute("pinColorOff",pinColorOff.name());
    element.setAttribute("pinColorOn",pinColorOn.name());

    QDomElement inp = element.ownerDocument().createElement("inputs");
    for (int i=0;i<fInputs.count();i++) {
        QDomElement ti = element.ownerDocument().createElement(tr("inp%1").arg(i));
        fInputs[i]->storeToStream(ti);
        inp.appendChild(ti);
    }
    element.appendChild(inp);
    QDomElement out = element.ownerDocument().createElement("outputs");
    for (int i=0;i<fOutputs.count();i++) {
        QDomElement ti = element.ownerDocument().createElement(tr("out%1").arg(i));
        fOutputs[i]->storeToStream(ti);
        out.appendChild(ti);
    }
    element.appendChild(out);
}

void QCPBase::doLogic()
{
    if (cpOwner->nodeLocks.contains(objectName()))
        return;
    if (isStateChanged()) {
        cpOwner->nodeLocks.append(objectName());
        doLogicPrivate();
        for (int i=0;i<fOutputs.count();i++) {
            if (fOutputs.at(i)->state!=fOutputs.at(i)->oldState)
                fOutputs.at(i)->applyState();
        }
        cpOwner->nodeLocks.removeAt(cpOwner->nodeLocks.indexOf(QRegExp(objectName())));
        update();
        savedState = calcState();
    }
}

void QCPBase::applyInputState(QCPInput *input, bool state)
{
    if (input==NULL) return;
    if (input->fromCmp!=NULL) {
        QUuid id = input->fromCmp->fOutputs[input->fromPin]->groupId;
        cpOwner->pendingLogicLinks.removeOne(id);
    }
    input->state = state;
    if (input->oldState == state) return;
    input->oldState = state;
    doLogic();
}

QCPOutput::QCPOutput(QObject * parent, QCPBase * aOwner, QString aPinName)
    : QObject(parent)
{
    pinName=aPinName;
    inversed=false;
    toPin=-1;
    toCmp=0;
    state=false;
    oldState=false;
    relCoord=QPoint(0,0);
    ownerCmp=aOwner;
    groupId=QUuid();
}

void QCPOutput::readFromStream(QTextStream &errlog, const QDomElement &element)
{
    bool ok;
    toPin = element.attribute("toPin","0").toInt(&ok);
    if (!ok) {
        errlog << tr("QCPOutput: cannot read pin number");
        toPin = 0;
    }
    toCmp=NULL;
    QString q = element.attribute("toCmp","NONE");
    inversed = (element.attribute("inversed","0").toInt(&ok)!=0);
    if (!ok) {
        errlog << tr("QCPInput: cannot read pin inversion mark");
        inversed = false;
    }
    if (q!="NONE")
        ffLogic=q;
    else
        ffLogic="";
    groupId=QUuid();
}

void QCPOutput::storeToStream(QDomElement &element )
{
    element.setAttribute("toPin",toPin);
    QString q;
    if (toCmp==0)
        q="NONE";
    else
        q=toCmp->objectName();
    element.setAttribute("toCmp",q);
    element.setAttribute("inversed",(int)inversed);
}

void QCPOutput::postLoadBind(QTextStream &errlog)
{
    if (ffLogic=="") return;
    QCPBase* b=ownerCmp->cpOwner->findChild<QCPBase *>(ffLogic);
    if (b==0)
        errlog << tr("QCPOutput: binding error in component %1").arg(ownerCmp->objectName());
    else {
        toCmp=b;
        oldState = !state;
        applyState();
    }
}

void QCPOutput::applyState()
{
    if (state==oldState) return;
    if (toCmp!=NULL) {
        if (!groupId.isNull()) {
            if (ownerCmp->cpOwner->pendingLogicLinks.contains(groupId)) {
                toCmp->fInputs[toPin]->state = state;
                toCmp->fInputs[toPin]->oldState = state;
            } else {
                ownerCmp->cpOwner->pendingLogicLinks.append(groupId);
                toCmp->fInputs[toPin]->applyState(state);
            }
        } else
            toCmp->fInputs[toPin]->applyState(state);
    }
    oldState = state;
}

QCPInput::QCPInput(QObject * parent, QCPBase * aOwner, QString aPinName)
    : QObject(parent)
{
    pinName=aPinName;
    inversed=false;
    fromPin=-1;
    fromCmp=0;
    state=false;
    relCoord=QPoint(0,0);
    ownerCmp=aOwner;
    connect(this,SIGNAL(applyInputState(QCPInput*,bool)),
            aOwner,SLOT(applyInputState(QCPInput*,bool)),
            Qt::QueuedConnection);
}

void QCPInput::readFromStream(QTextStream &errlog, const QDomElement &element)
{
    bool ok;
    fromPin = element.attribute("fromPin","0").toInt(&ok);
    if (!ok) {
        errlog << tr("QCPInput: cannot read pin number");
        fromPin = 0;
    }
    fromCmp=NULL;
    QString q = element.attribute("fromCmp","NONE");
    inversed = element.attribute("inversed","0").toInt(&ok);
    if (!ok) {
        errlog << tr("QCPInput: cannot read pin inversion mark");
        inversed = false;
    }
    if (q!="NONE")
        ffLogic=q;
    else
        ffLogic="";
}

void QCPInput::storeToStream(QDomElement & element)
{
    element.setAttribute("fromPin",fromPin);
    QString q;
    if (fromCmp==0)
        q="NONE";
    else
        q=fromCmp->objectName();
    element.setAttribute("fromCmp",q);
    element.setAttribute("inversed",(int)inversed);
}

void QCPInput::postLoadBind(QTextStream &errlog)
{
    if (ffLogic=="") return;
    QCPBase* b=ownerCmp->cpOwner->findChild<QCPBase *>(ffLogic);
    if (b==0)
        errlog << tr("QCPInput: binding error in component %1").arg(ownerCmp->objectName());
    else
        fromCmp=b;
}

void QCPInput::applyState(bool aState)
{
    emit applyInputState(this,aState);
}
