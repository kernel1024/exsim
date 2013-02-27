#include "cpled.h"

QCPLed::QCPLed(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
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
    return QSize(45*zoom()/100+getPinSize()+3*zoom()/100,
                 45*zoom()/100);
}

void QCPLed::readFromXML(QTextStream &errlog, const QDomElement &element)
{
    onColor = QColor(element.attribute("onColor","green"));
    offColor = QColor(element.attribute("offColor","darkgreen"));
    if (!onColor.isValid()) {
        errlog << tr("QCPLed: onColor value incorrect") << endl;
        onColor = QColor(Qt::green);
    }
    if (!offColor.isValid()) {
        errlog << tr("QCPLed: offColor value incorrect") << endl;
        onColor = QColor(Qt::darkGreen);
    }
    QCPBase::readFromXML(errlog,element);
}

void QCPLed::storeToXML(QDomElement &element)
{
    element.setAttribute("onColor",onColor.name());
    element.setAttribute("offColor",offColor.name());
    QCPBase::storeToXML(element);
}

void QCPLed::realignPins(QPainter &)
{
    fInp->relCoord=QPoint(getPinSize()/2,height()/2);
}

void QCPLed::doLogicPrivate()
{
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

    rc = rect();
    rc.adjust(getPinSize()+3*zoom()/100,0,-1,-1);
    p.setBrush(QBrush(Qt::black,Qt::SolidPattern));
    p.drawRect(rc);

    if (fInp->state) {
        p.setPen(QPen(onColor));
        p.setBrush(QBrush(onColor,Qt::SolidPattern));
    } else {
        p.setPen(QPen(offColor));
        p.setBrush(QBrush(offColor,Qt::SolidPattern));
    }
    rc.adjust(5*zoom()/100,5*zoom()/100,-5*zoom()/100,-5*zoom()/100);
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
