#include "cpbutton.h"
#include "renderarea.h"

QCPButton::QCPButton(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    pressed = false;
    oldPressed = false;
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
    return QSize(5*QApplication::fontMetrics().width("OFF") * cpOwner->zoom/100,
                 3*QApplication::fontMetrics().height()     * cpOwner->zoom/100);
}

void QCPButton::realignPins(QPainter &)
{
    fOut->relCoord=QPoint(width()-getPinSize()/2,height()/2);
}

void QCPButton::doLogicPrivate()
{
    fOut->state = pressed;
    oldPressed = pressed;
}

void QCPButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button()==Qt::LeftButton) {
        QRect rc = rect();
        rc.adjust(5,5,-5,-5);
        if (rc.contains(event->pos())) {
            pressed = !pressed;
            doLogic();
        }
    }
    QCPBase::mousePressEvent(event);
}

bool QCPButton::isStateChanged()
{
    return (pressed != oldPressed);
}

void QCPButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QPen op=p.pen();
    QBrush ob=p.brush();
    QFont of=p.font();

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
        QPoint(width(), 0),
        QPoint(width()-getPinSize(), getPinSize()),
        QPoint(getPinSize(), getPinSize()),
        QPoint(getPinSize(), height()-getPinSize()),
        QPoint(0, height())
    };
    p.setBrush(QBrush(usd,Qt::SolidPattern));
    p.drawPolygon(points1,6);
    // lower shadow
    const QPoint points2[6] = {
        QPoint(width(), 0),
        QPoint(width()-getPinSize(), getPinSize()),
        QPoint(width()-getPinSize(), height()-getPinSize()),
        QPoint(getPinSize(), height()-getPinSize()),
        QPoint(0, height()),
        QPoint(width(), height())
    };
    p.setBrush(QBrush(dsd,Qt::SolidPattern));
    p.drawPolygon(points2,6);

    redrawPins(p);

    QRect rc = rect();
    rc.adjust(5,5,-5,-5);
    p.setPen(QPen(Qt::black));
    p.setBrush(QBrush(QApplication::palette(cpOwner).window()));
    p.drawRect(rc);

    QFont n=of;
    n.setBold(true);
    n.setPointSize(n.pointSize()+2);
    p.setFont(n);
    if (pressed)
        p.drawText(rc,Qt::AlignCenter,"ON");
    else
        p.drawText(rc,Qt::AlignCenter,"OFF");

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);
}
