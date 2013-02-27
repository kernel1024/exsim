#include "cpbutton.h"
#include "renderarea.h"

QCPButton::QCPButton(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    pressed = false;
    savedClick = QPoint();
    pushButton = false;
    fOut = new QCPOutput(this,this);
    fOutputs.append(fOut);
}

QCPButton::~QCPButton()
{
    fOutputs.clear();
    delete fOut;
}

QSize QCPButton::minimumSizeHint() const
{
    return QSize(4*QApplication::fontMetrics().width("OFF") * zoom()/100,
                 3*QApplication::fontMetrics().height()     * zoom()/100);
}

void QCPButton::readFromStream(QTextStream &errlog, const QDomElement &element)
{
    bool ok;
    pushButton = (element.attribute("pushButton","0").toInt(&ok)!=0);
    if (!ok) {
        errlog << tr("QCPButton: cannot convert pushButton parameter");
        pushButton = false;
    }
    QCPBase::readFromStream(errlog,element);
}

void QCPButton::storeToStream(QDomElement &element)
{
    element.setAttribute("pushButton",(int)pushButton);
    QCPBase::storeToStream(element);
}

void QCPButton::realignPins(QPainter &)
{
    fOut->relCoord=QPoint(width()-getPinSize()/2,height()/2);
}

void QCPButton::doLogicPrivate()
{
    fOut->state = pressed;
}

void QCPButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button()==Qt::LeftButton) {
        savedClick=mapToGlobal(event->pos());
        if (pushButton) {
            QRect rc = rect();
            rc.adjust(getPinSize(),getPinSize(),-2*getPinSize(),-getPinSize());
            QPoint dp = mapToGlobal(event->pos()) - savedClick;
            if (rc.contains(event->pos()) && (dp.manhattanLength()<3)) {
                pressed = true;
                doLogic();
            }
        };
    }
    QCPBase::mousePressEvent(event);
}

void QCPButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button()==Qt::LeftButton) {
        if (pushButton) {
            if (pressed) {
                pressed = false;
                doLogic();
            }
        } else {
            QRect rc = rect();
            rc.adjust(getPinSize(),getPinSize(),-2*getPinSize(),-getPinSize());
            QPoint dp = mapToGlobal(event->pos()) - savedClick;
            if (rc.contains(event->pos()) && (dp.manhattanLength()<3)) {
                pressed = !pressed;
                doLogic();
            }
        }
    }
    QCPBase::mouseReleaseEvent(event);
}

void QCPButton::paintEvent(QPaintEvent *)
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

    rc = rect();
    rc.adjust(0,0,-getPinSize()-1,0);
    p.setPen(QPen(Qt::NoPen));
    QColor usd = Qt::lightGray;
    QColor dsd = Qt::darkGray;
    if (pressed) {
        usd = Qt::darkGray;
        dsd = Qt::lightGray;
    }
    // upper shadow
    const QPoint points1[6] = {
        QPoint(0, 0),
        QPoint(rc.width(), 0),
        QPoint(rc.width()-getPinSize(), getPinSize()),
        QPoint(getPinSize(), getPinSize()),
        QPoint(getPinSize(), rc.height()-getPinSize()),
        QPoint(0, rc.height())
    };
    p.setBrush(QBrush(usd,Qt::SolidPattern));
    p.drawPolygon(points1,6);
    // lower shadow
    const QPoint points2[6] = {
        QPoint(rc.width(), 0),
        QPoint(rc.width()-getPinSize(), getPinSize()),
        QPoint(rc.width()-getPinSize(), rc.height()-getPinSize()),
        QPoint(getPinSize(), rc.height()-getPinSize()),
        QPoint(0, rc.height()),
        QPoint(rc.width(), rc.height())
    };
    p.setBrush(QBrush(dsd,Qt::SolidPattern));
    p.drawPolygon(points2,6);

    redrawPins(p);

    rc.adjust(getPinSize()/2,getPinSize()/2,-getPinSize()/2,-getPinSize()/2);
    p.setPen(QPen(Qt::black));
    p.setBrush(QBrush(QApplication::palette(cpOwner).window()));
    p.drawRect(rc);

    QFont n=QApplication::font();
    n.setBold(true);
    n.setPointSize((n.pointSize()+2) * zoom()/100);
    p.setFont(n);
    if (pressed)
        p.drawText(rc,Qt::AlignCenter,"ON");
    else
        p.drawText(rc,Qt::AlignCenter,"OFF");

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);
}

void QCPButton::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu cm(this);
    QAction* acm;
    acm = cm.addAction(tr("Tumbler"),this,SLOT(modeTumbler()));
    acm->setCheckable(true);
    acm->setChecked(!pushButton);
    acm = cm.addAction(tr("Push button"),this,SLOT(modePushButton()));
    acm->setCheckable(true);
    acm->setChecked(pushButton);
    cm.exec(event->globalPos());
}

void QCPButton::modeTumbler()
{
    pushButton = false;
}

void QCPButton::modePushButton()
{
    pushButton = true;
}
