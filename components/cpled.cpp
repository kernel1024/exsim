#include "cpled.h"

QCPLed::QCPLed(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    state = false;
    oldState = false;
    onColor = Qt::green;
    offColor = Qt::darkGreen;
    fInp = new QCPInput(this,this);
    fInputs.append(fInp);
}

QCPLed::~QCPLed()
{
    fInputs.clear();
    delete fInp;
}

QSize QCPLed::minimumSizeHint() const
{
    return QSize(55,50);
}

void QCPLed::readFromStream(QDataStream &stream)
{
    QCPBase::readFromStream(stream);
    stream >> onColor >> offColor;
}

void QCPLed::storeToStream(QDataStream &stream)
{
    QCPBase::storeToStream(stream);
    stream << onColor << offColor;
}

void QCPLed::realignPins(QPainter &)
{
    fInp->relCoord=QPoint(getPinSize()/2,height()/2);
}

void QCPLed::doLogicPrivate()
{
    state = fInp->state;
    oldState = state;
}

bool QCPLed::isStateChanged()
{
    return (oldState != state);
}

void QCPLed::paintEvent(QPaintEvent *)
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

    if (state) {
        p.setPen(QPen(onColor));
        p.setBrush(QBrush(onColor,Qt::SolidPattern));
    } else {
        p.setPen(QPen(offColor));
        p.setBrush(QBrush(offColor,Qt::SolidPattern));
    }
    rc = rect();
    rc.adjust(12,7,-7,-7);
    p.drawEllipse(rc);

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);
}

void QCPLed::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu cm(this);
    cm.addAction(tr("Change ON color..."),this,SLOT(chooseColorOn()));
    cm.addAction(tr("Change OFF color..."),this,SLOT(chooseColorOff()));
    cm.exec(event->globalPos());
}

void QCPLed::chooseColorOn()
{
    QColor res = QColorDialog::getColor(onColor,this);
    if (res.isValid()) {
        onColor = res;
        update();
    }
}

void QCPLed::chooseColorOff()
{
    QColor res = QColorDialog::getColor(offColor,this);
    if (res.isValid()) {
        offColor = res;
        update();
    }
}
