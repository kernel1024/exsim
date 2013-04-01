#include "cpvcc.h"

QCPVcc::QCPVcc(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    fOut = new QCPOutput(this,this,"Q");
    fOut->state = true;
    fOutputs.append(fOut);
}

QCPVcc::~QCPVcc()
{
    fOutputs.clear();
    delete fOut;
}

QSize QCPVcc::minimumSizeHint() const
{
    QFont n=QApplication::font();
    n.setPointSize((n.pointSize()) * zoom()/100);
    QFontMetrics fm(n);
    return QSize(fm.width("  Vcc  Q")+2*getPinSize()*zoom()/100,
                 2*fm.height());
}

void QCPVcc::realignPins()
{
    fOut->relCoord=QPoint(width()-getPinSize()/2,height()/2);
}

void QCPVcc::doLogicPrivate()
{
    fOut->state = true;
}

void QCPVcc::paintEvent(QPaintEvent *)
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

    rc.adjust(0,0,-p.fontMetrics().width("Q")-getPinSize(),0);
    QFont n=QApplication::font();
    n.setPointSize((n.pointSize()) * zoom()/100);
    p.setFont(n);
    p.drawText(rc,Qt::AlignCenter,tr("Vcc"));

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);

    drawSelection(p);
}
