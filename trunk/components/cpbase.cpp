#include "renderarea.h"
#include "cpbase.h"

QCPBase::QCPBase(QWidget *parent, QRenderArea *aOwner)
    : QWidget(parent)
{
    pinColorOff=Qt::blue;
    pinColorOn=Qt::red;
    isDragging=false;
    cpOwner=aOwner;
    oldZoom=100;
    savedState=0;
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

int QCPBase::getPinSize()
{
    return 8*zoom()/100;
}

void QCPBase::postLoadBind()
{
    for (int i=0;i<fInputs.count();i++)
        fInputs[i]->postLoadBind();
    for (int i=0;i<fOutputs.count();i++)
        fOutputs[i]->postLoadBind();
}

void QCPBase::checkRecycle()
{
    if (!(frameGeometry().intersects(cpOwner->recycle->frameGeometry()))) return;
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

qint32 QCPBase::calcState()
{
    qint32 a = 0;
    for(int i=0;i<fInputs.count();i++)
        if (fInputs.at(i)->state)
            a = a | (1 << i);
    return a;
}

void QCPBase::mouseMoveEvent(QMouseEvent * event)
{
    //  if (event->button()==Qt::LeftButton)

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

void QCPBase::readFromStream( QDataStream & stream )
{
    stream >> pinColorOff >> pinColorOn;
    for (int i=0;i<fInputs.count();i++)
        fInputs[i]->readFromStream(stream);
    for (int i=0;i<fOutputs.count();i++)
        fOutputs[i]->readFromStream(stream);
}

void QCPBase::storeToStream( QDataStream & stream )
{
    stream << pinColorOff << pinColorOn;
    for (int i=0;i<fInputs.count();i++)
        fInputs[i]->storeToStream(stream);
    for (int i=0;i<fOutputs.count();i++)
        fOutputs[i]->storeToStream(stream);
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
    int idx = fInputs.indexOf(input);
    if (idx<0) return;
    fInputs[idx]->state = state;
    if (fInputs[idx]->oldState == state) return;
    fInputs[idx]->oldState = state;
    doLogic();
}

QCPOutput::QCPOutput(QObject * parent, QCPBase * aOwner)
    : QObject(parent)
{
    pinName=QString();
    inversed=false;
    toPin=-1;
    toCmp=0;
    state=false;
    oldState=false;
    relCoord=QPoint(0,0);
    ownerCmp=aOwner;
}

void QCPOutput::readFromStream( QDataStream & stream )
{
    stream >> toPin;
    toCmp=0;
    QString q;
    stream >> q;
    stream >> inversed;
    if (q!="<NONE>")
        ffLogic=q;
    else
        ffLogic="";
}

void QCPOutput::storeToStream( QDataStream & stream )
{
    stream << toPin;
    QString q;
    if (toCmp==0)
        q="<NONE>";
    else
        q=toCmp->objectName();
    stream << q;
    stream << inversed;
}

void QCPOutput::postLoadBind()
{
    if (ffLogic=="") return;
    QCPBase* b=ownerCmp->cpOwner->findChild<QCPBase *>(ffLogic);
    if (b==0)
        throw "Binding error. Error in loading components from file.";
    else
        toCmp=b;
}

void QCPOutput::applyState()
{
    if (state==oldState) return;
    if (toCmp!=NULL)
        toCmp->fInputs[toPin]->applyState(state);
    oldState = state;
}

QCPInput::QCPInput(QObject * parent, QCPBase * aOwner)
    : QObject(parent)
{
    pinName=QString();
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

void QCPInput::readFromStream( QDataStream & stream )
{
    stream >> fromPin;
    fromCmp=0;
    QString q;
    stream >> q;
    stream >> inversed;
    if (q!="<NONE>")
        ffLogic=q;
    else
        ffLogic="";
}

void QCPInput::storeToStream( QDataStream & stream )
{
    stream << fromPin;
    QString q;
    if (fromCmp==0)
        q="<NONE>";
    else
        q=fromCmp->objectName();
    stream << q;
    stream << inversed;
}

void QCPInput::postLoadBind()
{
    if (ffLogic=="") return;
    QCPBase* b=ownerCmp->cpOwner->findChild<QCPBase *>(ffLogic);
    if (b==0)
        throw "Binding error. Error in loading components from file.";
    else
        fromCmp=b;
}

void QCPInput::applyState(bool aState)
{
    emit applyInputState(this,aState);
}
